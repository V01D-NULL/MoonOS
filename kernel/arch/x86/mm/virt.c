#include "virt.h"
#include <base/assert.h>
#include <moon-io/serial.h>
#include <moon-sys/handover.h>
#include <panic.h>
#include "page.h"
#include "phys.h"
static struct Pml *kernel_pagemap;

void arch_init_paging(BootHandover handover)
{
    assert((kernel_pagemap = arch_alloc_page()) != NULL);

    // arch_map_range(kernel_pagemap, RANGE(0, GiB(4)), MAP_KERN, 0);

    for (int i = 0; i < handover.memory_map.entry_count; i++)
    {
        auto entry = handover.memory_map.entries[i];

        for (size_t j = 0; j < entry->length / PAGE_SIZE; j++)
        {
            arch_map_page(
                kernel_pagemap,
                va(entry->base + j * PAGE_SIZE),
                entry->base + j * PAGE_SIZE,
                entry->type == HANDOVER_MEMMAP_RESERVED ||
                        entry->type == HANDOVER_MEMMAP_BAD_MEMORY ||
                        entry->type == HANDOVER_MEMMAP_KERNEL_AND_MODULES
                    ? MAP_READONLY
                    : MAP_KERN);
        }
    }

    for (size_t i = 0; i < handover.kernel_size; i++)
    {
        arch_map_page(kernel_pagemap,
                      handover.kernel_virt_start + i * PAGE_SIZE,
                      handover.kernel_phys_start + i * PAGE_SIZE,
                      MAP_KERN);
    }

    arch_switch_to_kernel_pagemap();
}

inline uint64_t index_of(uint64_t vaddr, int offset)
{
    return vaddr >> (12 + 9 * (offset - 1)) & 0x1FF;
}

// Get the next PML in the page table hierarchy
static struct Pml *vmm_pml_advance(struct Pml *entry, size_t level, int flags)
{
    auto pte = entry->page_tables[level];
    if (pte.present)
    {
        if ((flags >> 1) & 1 && !pte.readwrite)
        {
            pte.readwrite             = 1;
            entry->page_tables[level] = pte;
        }

        if ((flags >> 2) & 1 && !pte.supervisor)
        {
            pte.supervisor            = 1;
            entry->page_tables[level] = pte;
        }

        return (struct Pml *)va((uintptr_t)pte.address << PAGE_SHIFT);
    }

    uint64_t addr = (uint64_t)pa(arch_alloc_page());
    panic_if(!addr, "Failed to allocate memory for a pagetable!");

    entry->page_tables[level] = paging_create_entry(addr, flags);
    return (struct Pml *)va(addr);
}

void arch_map_page(struct Pml *pml4, virt_t vaddr, phys_t paddr, int flags)
{
    if (!pml4 || !flags)
    {
#if defined DEBUG_VMM || defined DEBUG_MM
        panic(
            "Tried to call %s while pml4 or flags are NULL/0! (PML4: 0x%lX | "
            "flags: %d)",
            __func__,
            (uint64_t)pml4,
            flags);
#endif
        return;
    }

    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = vmm_pml_advance(pml4, index_of(vaddr, 4), flags);
    pml2 = vmm_pml_advance(pml3, index_of(vaddr, 3), flags);
    pml1 = vmm_pml_advance(pml2, index_of(vaddr, 2), flags);

    pml1->page_tables[index_of(vaddr, 1)] = paging_create_entry(paddr, flags);

    invlpg(vaddr);
}

void arch_quickmap_page(struct Pml *pml4, virt_t vaddr, phys_t paddr, int flags)
{
    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = vmm_pml_advance(pml4, index_of(vaddr, 4), flags);
    pml2 = vmm_pml_advance(pml3, index_of(vaddr, 3), flags);
    pml1 = vmm_pml_advance(pml2, index_of(vaddr, 2), flags);

    pml1->page_tables[index_of(vaddr, 1)] = paging_create_entry(paddr, flags);
    invlpg(vaddr);
}

void arch_unmap_page(struct Pml *pml4, virt_t vaddr)
{
    if (!pml4)
        return;

    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = GET_PMLx(vaddr, pml4, 4);
    pml2 = GET_PMLx(vaddr, pml3, 3);
    pml1 = GET_PMLx(vaddr, pml2, 2);

    uint64_t addr = pml1->page_tables[index_of(vaddr, 1)].address;
    pml1->page_tables[index_of(vaddr, 1)] = paging_purge_entry();

    invlpg(vaddr);
    arch_free_page((void *)addr);
}

void arch_map_range(struct Pml *pml4, Range range, int flags, uint64_t offset)
{
    uint64_t base = range.base, limit = range.limit;
    assert(base % 4096 == 0 && limit % 4096 == 0);

    for (; base != limit; base += PAGE_SIZE)
    {
        arch_map_page(pml4, base + offset, base, flags);
    }
}

void arch_quickmap_range(struct Pml *pml4, Range range, int flags,
                         uint64_t offset)
{
    uint64_t base = range.base, limit = range.limit;
    assert(base % 4096 == 0 && limit % 4096 == 0);

    for (; base != limit; base += PAGE_SIZE)
    {
        arch_quickmap_page(pml4, base + offset, base, flags);
    }
}

void arch_switch_to_kernel_pagemap(void)
{
    wrcr3(pa(kernel_pagemap));
}

struct Pml *arch_create_new_pagemap(void)
{
    return (struct Pml *)arch_alloc_page();
}

void arch_copy_kernel_mappings(Task task)
{
    for (int i = 256; i < 512; i++)
        task.pagemap->page_tables[i] = kernel_pagemap->page_tables[i];
}

void arch_switch_pagemap(Task task)
{
    if (!task.pagemap)
    {
        panic_if(task.task_type == TASK_DAEMON,
                 "The kernel daemon task '%s' attempted to switch to a NULL "
                 "pagemap!",
                 task.descriptor);

        return;  // Todo: Should probably return a status code or something
    }

    // memcpy((uint8_t *)active_pagemap, (const uint8_t *)task.pagemap,
    // sizeof(*task.pagemap) / sizeof(task.pagemap[0]));
    wrcr3(pa(task.pagemap));
}

void arch_pagefault_handler(uint64_t cr2, int error_code)
{
    struct Pagefault pf = paging_get_pagefault_flags(error_code, true);

#ifdef DEBUG_VMM
    printk("vmm",
           "\033[93m%s pagefault @%p (error_code: %d)\033[39m\n",
           (error_code >> 4) & 1 ? "Userspace" : "KernelSpace",
           cr2,
           error_code);
    printk("vmm",
           "-- Pagefault flags: {\n"
           "-- Present:           %d\n"
           "-- Write:             %d\n"
           "-- User:              %d\n"
           "-- Instruction fetch: %d\n"
           "-- Protection key:    %d\n"
           "-- Shadow stack:      %d\n"
           "}\n",
           pf.present,
           pf.write,
           pf.user,
           pf.instruction_fetch,
           pf.protection_key,
           pf.shadow_stack);
#endif

    panic_if(pf.panic_on_unhandled,
             "Pagefault panic: Did not implement at least one of these:\n"
             "-- Pagefault flags: {\n"
             "-- Present:           %d\n"
             "-- Write:             %d\n"
             "-- User:              %d\n"
             "-- Instruction fetch: %d\n"
             "-- Protection key:    %d\n"
             "-- Shadow stack:      %d\n"
             "}\n",
             pf.present,
             pf.write,
             pf.user,
             pf.instruction_fetch,
             pf.protection_key,
             pf.shadow_stack);

    // Todo: Check if cr2 is a high vma and verify flags accordingly (Don't want
    // a user accessing kernel data structures :p)
    arch_map_page((struct Pml *)cr3_read(), cr2, cr2, pf.flags);
}

phys_t arch_translate(struct Pml *pml4, virt_t vaddr)
{
    if (!pml4)
    {
        panic("Tried to call %s with a NULL PML4!", __func__);
    }

    struct Pte *pml3_entry = &pml4->page_tables[index_of(vaddr, 4)];
    if (!pml3_entry->present)
        return 0;

    struct Pml *pml3 =
        (struct Pml *)(va((uintptr_t)pml3_entry->address << PAGE_SHIFT));

    struct Pte pml2_entry = pml3->page_tables[index_of(vaddr, 3)];
    if (!pml2_entry.present)
        return 0;

    struct Pml *pml2 =
        (struct Pml *)(va((uintptr_t)pml2_entry.address << PAGE_SHIFT));

    struct Pte pml1_entry = pml2->page_tables[index_of(vaddr, 2)];
    if (!pml1_entry.present)
        return 0;

    struct Pml *pml1 =
        (struct Pml *)(va((uintptr_t)pml1_entry.address << PAGE_SHIFT));

    struct Pte pte = pml1->page_tables[index_of(vaddr, 1)];
    if (!pte.present)
        return 0;

    // Combine the physical address with the offset within the page
    return (phys_t)va((pte.address << PAGE_SHIFT) | (vaddr & (PAGE_SIZE - 1)));
}
