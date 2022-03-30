#include "vmm.h"

#include <mm/pmm.h>
#include <mm/mm.h>
#include <libk/kassert.h>
#include <amd64/moon.h>
#include <stdbool.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <panic.h>
#include <devices/term/fallback/fterm.h>

// create_lock("vmm", vmm_lock);

static struct Pml *kernel_pagemap, *active_pagemap;

static struct Pml *vmm_pml_advance(
    struct Pml *entry,
    size_t level, int flags
);

void vmm_init(struct stivale2_mmap_entry *mmap, int entries)
{
    assert((kernel_pagemap = pmm_alloc()) != NULL);
    
    vmm_map_range(vmm_as_range(0, 4 * GB, $identity_vma), MAP_KERN, kernel_pagemap);
    vmm_map_range(vmm_as_range(0, 2 * GB, $high_vma), MAP_KERN, kernel_pagemap);
    vmm_map_range(vmm_as_range(0, 2 * GB, $high_vma_code), MAP_READONLY, kernel_pagemap);
    
    debug(true, "Old PML4: 0x%llx\n", cr_read(CR3)); // Bootloader pml4
    vmm_switch_to_kernel_pagemap();
    debug(true, "New PML4: 0x%llx\n", cr_read(CR3)); // Kernel pml4
}

inline bool check_flag(int flags, int bit) { return (flags >> bit) & 1; }

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
        return (struct Pml*)((uintptr_t)pte.address << PAGE_SHIFT);
    }

    uint64_t addr = (uint64_t)pmm_alloc();
    panic_if(!addr, "Failed to allocate memory for a pagetable!");
    entry->page_tables[level] = vmm_create_entry(addr, flags);
    return (struct Pml*)addr;
}

void vmm_map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags)
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

    pml1->page_tables[index_of(vaddr, 1)] = vmm_create_entry(paddr, flags);
    invlpg(vaddr);
}

void vmm_unmap(struct Pml *pml4, size_t vaddr)
{
    if (!pml4)
        return;

    struct Pml *pml3, *pml2, *pml1 = NULL;
    pml3 = GET_PMLx(vaddr, pml4, 4);
    pml2 = GET_PMLx(vaddr, pml3, 3);
    pml1 = GET_PMLx(vaddr, pml2, 2);

    uint64_t addr = pml1->page_tables[index_of(vaddr, 1)].address;
    pml1->page_tables[index_of(vaddr, 1)] = vmm_purge_entry();
    
    invlpg(vaddr);
    pmm_free((void*)addr);
}

struct pte vmm_create_entry(uint64_t paddr, int flags)
{
    return (struct pte) {
        .present = check_flag(flags, 0),
        .readwrite = check_flag(flags, 1),
        .supervisor = check_flag(flags, 2),
        .writethrough = 0,
        .cache_disabled = 0,
        .accessed = 0,
        .dirty = 0,
        .ignore = 0,
        .global = 0,
        .avail = 0,
        .address = paddr >> PAGE_SHIFT
    };
}

struct pte vmm_purge_entry()
{
    return vmm_create_entry(0, 0);
}

void vmm_switch_to_kernel_pagemap(void)
{
    wrcr3(kernel_pagemap);
}

void vmm_map_range(VmmRange range, int flags, struct Pml *pagemap)
{
    uint64_t base = range.range.base, limit = range.range.limit;
    assert(base % 4096 == 0 && limit % 4096 == 0);

    for (; base != limit; base += PAGE_SIZE)
    {
        vmm_map(pagemap, base + range.address_offset, base, flags);
    }
}

void vmm_pagefault_handler(uint64_t cr2, int error_code)
{
#ifdef DEBUG_VMM
    printk("vmm", "\033[93m%s pagefault @%p (error_code: %d)\033[39m\n", error_code & 4 ? "Userspace" : "KernelSpace", cr2, error_code);
#endif

    // int flags = check_flag(error_code, 0); // Present
    // flags |= check_flag(error_code, 1); // Write
    // flags |= check_flag(error_code, 2); // User (CPL=3)
    
    // debug(true, "flags: %d | err: %d\n", flags, error_code);

    // Todo: Check if cr2 is a high vma and add the offset accordingly.
    vmm_map(active_pagemap, cr2, cr2, error_code); // This is just a hack but should cover generic faults. I don't feel like fixing this just yet.
}

struct Pml *vmm_get_kernel_pagemap(void)
{
    return kernel_pagemap;
}

struct Pml *vmm_create_new_pagemap(void)
{
    return (struct Pml *)pmm_alloc();
}

void vmm_copy_kernel_mappings(task_t task)
{
    for (int i = 0; i < 256; i++)
        task.pagemap->page_tables[i] = vmm_purge_entry();

    for (int i = 256; i < 512; i++)
        task.pagemap->page_tables[i] = kernel_pagemap->page_tables[i];
}

void vmm_switch_pagemap(task_t task)
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
