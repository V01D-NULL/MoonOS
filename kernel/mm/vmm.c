#include "vmm.h"
#include "memdefs.h"
#include "libk/kassert.h"
#include "../amd64/validity.h"
#include "../drivers/gfx/gfx.h"
#include "../int/gdt.h"
#include "../int/idt.h"
#include "../util/ptr.h"
#include <stdbool.h>

#define GB 0x40000000UL

// struct page_directory pml4;
// uint64_t *pml4;

static uint64_t *vmm_get_pml(
    uint64_t *entry,
    size_t level, int flags
);

void identity_map(uint64_t *pml4, size_t start, size_t end, int flags)
{
    for (size_t i = start; i < end; i += PAGE_SIZE)
    {
        vmm_map(pml4, i, i, flags);
    }
}

void vmm_init()
{
    init_gdt();
    init_idt();

    uint64_t *pml4 = (uint64_t*)from_virt(GENERIC_CAST(uintptr_t, pmm_alloc()));
    printk("vmm", "pml4 resides at 0x%llx\n", pml4);
    
    // Identity map first 4 GB
    identity_map(pml4, 0, 4 * GB, FLAGS_PR | FLAGS_RW);

    //Map physical address 0x0-4GB to virtual address 0xffff800000000000
    // for (uintptr_t i = 0; i < 4 * GB; i += PAGE_SIZE)
    // {
    //     vmm_map(0xffff800000000000UL + i, i, FLAGS_RW);
    // }

    // for (uintptr_t i = 0; i < 0x80000000; i += PAGE_SIZE)
    // {
    //     vmm_map(0xffffffff80000000, i, FLAGS_RW);
    // }

    debug(true, "Old PML4: %llx\n", cr_read(CR3)); // Bootloader pml4
    PAGE_LOAD_CR3(GENERIC_CAST(uint64_t, pml4));
    debug(true, "New PML4: %llx\n", cr_read(CR3)); // Kernel pml4

    printk("vmm", "Initialised vmm\n");
}

static uint64_t *vmm_get_pml(uint64_t *entry, size_t level, int flags)
{
    if (entry[level] & 1)
    {
        return (uint64_t*)((size_t)entry[level] & ~(0x1FF)) - VMM_BASE;
    }
    else
    {
        // entry[level] = VOID_PTR_TO_64(pmm_alloc());
        debug(true, "alloc(): 0x%lX\n", pmm_alloc());
        entry[level] |= flags;
        return GENERIC_CAST(uint64_t*, entry[level]);
    }
}

void vmm_map(uint64_t *pml4, size_t vaddr, size_t paddr, int flags)
{
    //TODO:
    //Performance tweak: Use an algorithm to save an address
    //which has been mapped already and check if the address is already mapped here
    page_info_t info = vmm_dissect_vaddr(vaddr);

    uint64_t *pml3, *pml2, *pml1 = NULL;
    pml3 = vmm_get_pml(pml4, info.lv4, flags);
    pml2 = vmm_get_pml(pml3, info.lv3, flags);
    pml1 = vmm_get_pml(pml2, info.lv2, flags);
    pml1[info.lv1] = (paddr | flags);
}

page_info_t vmm_dissect_vaddr(uint64_t virt_addr)
{
    page_info_t pg_info;
    const int bitmask = 0x1FF;

    pg_info.page_offset = virt_addr & 0xfff;
    virt_addr >>= 12;

    pg_info.lv1 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv2 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv3 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv4 = virt_addr & bitmask;

    return pg_info;
}
