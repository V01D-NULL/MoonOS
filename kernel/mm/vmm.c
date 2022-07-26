#include "vmm.h"
#include <mm/pmm.h>
#include <mm/mm.h>
#include <libk/kassert.h>
#include <amd64/moon.h>
#include <ktypes.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <panic.h>

// create_lock("vmm", vmm_lock);

static struct Pml *kernel_pagemap;

static struct Pml *vmm_pml_advance(
    struct Pml *entry,
    size_t level, int flags);

void v_init(struct stivale2_mmap_entry *mmap, int entries)
{
    assert((kernel_pagemap = pmm_alloc()) != NULL);

    v_map_range_fast(as_vm_range(0, GiB(4), $identity_vma), MAP_KERN, kernel_pagemap);
        
    v_map_range_fast(as_vm_range(0, GiB(4), $high_vma), MAP_KERN, kernel_pagemap);
    v_map_range_fast(as_vm_range(0, GiB(4), $high_vma_heap), MAP_KERN, kernel_pagemap);
    v_map_range_fast(as_vm_range(0, GiB(2), $high_vma_code), MAP_READONLY, kernel_pagemap);

    debug(true, "Old PML4: 0x%llx\n", cr_read(CR3)); // Bootloader pml4
    switch_to_kernel_pagemap();
    debug(true, "New PML4: 0x%llx\n", cr_read(CR3)); // Kernel pml4
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
        return (struct Pml *)((uintptr_t)pte.address << PAGE_SHIFT);
    }

    uint64_t addr = (uint64_t)pmm_alloc();
    panic_if(!addr, "Failed to allocate memory for a pagetable!");

    entry->page_tables[level] = paging_create_entry(addr, flags);
    return (struct Pml *)addr;
}

void v_map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags)
{
    if (!pml4 || !flags)
    {
#if defined DEBUG_VMM || defined DEBUG_MM
        panic("Tried to call %s while pml4 or flags are NULL/0! (PML4: 0x%lX | flags: %d)", __func__, (uint64_t)pml4, flags);
#endif
        return;
    }

    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = vmm_pml_advance(pml4, index_of(vaddr, 4), flags);
    pml2 = vmm_pml_advance(pml3, index_of(vaddr, 3), flags);
    pml1 = vmm_pml_advance(pml2, index_of(vaddr, 2), flags);

    struct pte *L1 = &pml1->page_tables[index_of(vaddr, 1)];
    if (L1->address != 0)
        return;

    *L1 = paging_create_entry(paddr, flags);
    invlpg(vaddr);
}

// It's v_map, but without checks or comparisons to save cpu cycles
void v_map_fast(struct Pml *pml4, size_t vaddr, size_t paddr, int flags)
{
    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = vmm_pml_advance(pml4, index_of(vaddr, 4), flags);
    pml2 = vmm_pml_advance(pml3, index_of(vaddr, 3), flags);
    pml1 = vmm_pml_advance(pml2, index_of(vaddr, 2), flags);

    pml1->page_tables[index_of(vaddr, 1)] = paging_create_entry(paddr, flags);
    invlpg(vaddr);
}

void v_unmap(struct Pml *pml4, size_t vaddr)
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
    pmm_free((void *)addr);
}

void switch_to_kernel_pagemap(void)
{
    wrcr3(kernel_pagemap);
}

void v_map_range(VmmRange range, int flags, struct Pml *pagemap)
{
    uint64_t base = range.range.base, limit = range.range.limit;
    assert(base % 4096 == 0 && limit % 4096 == 0);

    for (; base != limit; base += PAGE_SIZE)
    {
        v_map(pagemap, base + range.address_offset, base, flags);
    }
}

// Same as v_map_range except that it uses v_map_fast and does not perform any alignment checks
void v_map_range_fast(VmmRange range, int flags, struct Pml *pagemap)
{
    uint64_t base = range.range.base, limit = range.range.limit;
    for (; base != limit; base += PAGE_SIZE)
    {
        v_map_fast(pagemap, base + range.address_offset, base, flags);
    }
}


void pagefault_handler(uint64_t cr2, int error_code)
{
    struct Pagefault pf = paging_get_pagefault_flags(error_code, true);

#ifdef DEBUG_VMM
    printk("vmm", "\033[93m%s pagefault @%p (error_code: %d)\033[39m\n", (error_code >> 4) & 1 ? "Userspace" : "KernelSpace", cr2, error_code);
    printk("vmm", "-- Pagefault flags: {\n"
                  "-- Present:           %d\n"
                  "-- Write:             %d\n"
                  "-- User:              %d\n"
                  "-- Instruction fetch: %d\n"
                  "-- Protection key:    %d\n"
                  "-- Shadow stack:      %d\n"
                  "}\n",
           pf.present, pf.write, pf.user,
           pf.instruction_fetch,
           pf.protection_key, pf.shadow_stack);
#endif

    panic_if(pf.panic_on_unhandled, "Pagefault panic: Did not implement at least one of these:\n"
            "-- Pagefault flags: {\n"
                  "-- Present:           %d\n"
                  "-- Write:             %d\n"
                  "-- User:              %d\n"
                  "-- Instruction fetch: %d\n"
                  "-- Protection key:    %d\n"
                  "-- Shadow stack:      %d\n"
                  "}\n",
           pf.present, pf.write, pf.user,
           pf.instruction_fetch,
           pf.protection_key, pf.shadow_stack
    );


    // Todo: Check if cr2 is a high vma and verify flags accordingly (Don't want a user accessing kernel data structures :p)
    v_map((struct Pml*)cr_read(CR3), cr2, cr2, pf.flags);
}

struct Pml *get_kernel_pagemap(void)
{
    return kernel_pagemap;
}

struct Pml *create_new_pagemap(void)
{
    return (struct Pml *)pmm_alloc();
}

void copy_kernel_mappings(task_t task)
{
    for (int i = 0; i < 256; i++)
        task.pagemap->page_tables[i] = paging_purge_entry();

    for (int i = 256; i < 512; i++)
        task.pagemap->page_tables[i] = kernel_pagemap->page_tables[i];
}

void switch_pagemap(task_t task)
{
    if (!task.pagemap)
    {
        panic_if(
            task.task_type == TASK_DAEMON,
            "The kernel daemon task '%s' attempted to switch to a NULL pagemap!", task.descriptor);

        return; // Todo: Should probably return a status code or something
    }

    // memcpy((uint8_t *)active_pagemap, (const uint8_t *)task.pagemap, sizeof(*task.pagemap) / sizeof(task.pagemap[0]));
    wrcr3(task.pagemap);
}
