#include "vmm.h"
#include "mm.h"
#include <libk/kassert.h>
#include <amd64/moon.h>
#include <int/gdt.h>
#include <int/idt.h>
#include <util/ptr.h>
#include <stdbool.h>
#include <trace/sym.h>
#include <printk.h>
#include <panic.h>
#include <proc/smp/spinlock.h>
#include <amd64/paging/memory/pat.h>
#include <amd64/paging/paging.h>
#include <devices/term/fallback/fterm.h>

#define WC (1 << 3) | (1 << 4) // pat0, pat1

create_lock("vmm", vmm_lock);

static uint64_t *kernel_pagemap;
static uint64_t *active_pagemap;
static bool la57_enabled = false;

STATIC_INLINE uint64_t index_of(uint64_t vaddr, int offset)
{
    return vaddr >> (12 + 9 * (offset - 1)) & 0x1FF;
}

void vmm_init(bool has_5_level_paging, struct stivale2_struct_tag_memmap *mmap)
{
    configure_pat();
    la57_enabled = has_5_level_paging;

    void *page = pmm_alloc();
    panic_if(page == NULL, "Cannot allocate kernel pagemap");
    kernel_pagemap = (uint64_t *)page;
    debug(true, "kpagemap = %llx\n", kernel_pagemap);

    fterm_write("vmm: Mapping the framebuffer and bootloader reclaimable entries...\n");

    // This mapping is required in order to use the stivale2 terminal
    for (size_t i = 0; i < mmap->entries; i++)
    {
        size_t base = mmap->memmap[i].base;
        size_t top = base + mmap->memmap[i].length;
        size_t type = mmap->memmap[i].type;

        if (type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
        {
            fterm_write("vmm: Identity mapping BRE (0x%lX-0x%lX) [Memory caching type: UC | Access flags: RO]\n", base, top);
            vmm_map_range(vmm_as_range(base, top, VMEM_DIRECT_MAPPING), MAP_READONLY, NULL);
        }
        if (type == STIVALE2_MMAP_FRAMEBUFFER)
        {
            fterm_write("vmm: Identity mapping framebuffer (0x%lX-0x%lX) [Memory caching type: UC | Access flags: RO]\n", base, top);
            vmm_map_range(vmm_as_range(base, top, VMEM_DIRECT_MAPPING), MAP_READONLY | WC, NULL);
        }
    }

    // Identity map 0-4GiB
    fterm_write("vmm: Identity mapping 0-4GiB [Memory caching type: UC | Access flags Kernel+RW]\n");
    vmm_map_range(vmm_as_range(0, 4 * GB, VMEM_DIRECT_MAPPING), MAP_KERN, NULL);

    // Map 2GiB of kernel data
    fterm_write("vmm: Mapping 2GiB of kernel data at offset 0x%s [Memory caching type: UC | Access flags Kernel+RW]\n", la57_enabled ? "FF0000..." : "FFFF80...");
    vmm_map_range(vmm_as_range(0, 2 * GB, la57_enabled ? VMEM_LV5_BASE : VMEM_LV4_BASE), MAP_KERN, NULL);

    // Map 2GiB of kernel code
    fterm_write("vmm: Mapping 2GiB of kernel code [Memory caching type: UC | Access flags: Kernel+RO]\n");
    vmm_map_range(vmm_as_range(0, 2 * GB, VMEM_CODE_BASE), MAP_READONLY, NULL);

    fterm_write("vmm: Bootloader pagemap: 0x%lX\nvmm: Kernel pagemap: 0x%lX\n", cr_read(CR3), kernel_pagemap);
    wrcr3(GENERIC_CAST(uint64_t, kernel_pagemap));
    debug(true, "New PML4: %llx\n", cr_read(CR3)); // Kernel pagemap
}

static uint64_t *vmm_get_pml_or_alloc(uint64_t *entry, size_t level, int flags)
{
    if (entry[level] & 1)
    {
        entry[level] |= flags;
        goto no_alloc;
    }

    void *page = pmm_alloc();
    panic_if(page == NULL, "page == NULL!");

    entry[level] = (uintptr_t)page;
    entry[level] |= flags;

no_alloc:
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

static uint64_t *vmm_get_pml(uint64_t *entry, size_t level)
{
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

void vmm_map(uint64_t *pagemap, size_t vaddr, size_t paddr, int flags)
{
    if (pagemap == NULL)
        pagemap = kernel_pagemap;

    acquire_lock(&vmm_lock);
    uint64_t lv4 = index_of(vaddr, 4);
    uint64_t lv3 = index_of(vaddr, 3);
    uint64_t lv2 = index_of(vaddr, 2);
    uint64_t lv1 = index_of(vaddr, 1);
    int pat_attr = get_pat();

    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;

        pml4 = vmm_get_pml_or_alloc(pagemap, lv5, flags);
        paging_cache_disable_set(pml4, lv4, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml4, lv4, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml4, lv4, pat_attr & (1 << 12));

        pml3 = vmm_get_pml_or_alloc(pml4, lv4, flags);
        paging_cache_disable_set(pml3, lv3, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml3, lv3, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml3, lv3, pat_attr & (1 << 12));

        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        paging_cache_disable_set(pml2, lv2, pat_attr & PG_PCD);
        paging_write_through_set(pml2, lv2, pat_attr & PG_PWT);
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml2, lv2, pat_attr & PG_PAT);

        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);

        paging_cache_disable_set(pml1, lv1, pat_attr & PG_PCD);
        paging_write_through_set(pml1, lv1, pat_attr & PG_PWT);
        paging_pat_set(pml1, lv1, pat_attr & PG_PAT);
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(pagemap, lv4, flags);

        paging_cache_disable_set(pml3, lv3, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml3, lv3, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml3, lv3, pat_attr & (1 << 12));

        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        paging_cache_disable_set(pml2, lv2, pat_attr & PG_PCD);
        paging_write_through_set(pml2, lv2, pat_attr & PG_PWT);
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml2, lv2, pat_attr & (1 << 12));

        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);
        paging_cache_disable_set(pml1, lv1, pat_attr & PG_PCD);
        paging_write_through_set(pml1, lv1, pat_attr & PG_PWT);
        paging_pat_set(pml1, lv1, pat_attr & (1 << 7));
    }

    invlpg(vaddr);
    release_lock(&vmm_lock);
}

void vmm_unmap(uint64_t *pagemap, size_t vaddr)
{
    if (pagemap == NULL)
        pagemap = kernel_pagemap;

    acquire_lock(&vmm_lock);
    uint64_t lv4 = index_of(vaddr, 4);
    uint64_t lv3 = index_of(vaddr, 3);
    uint64_t lv2 = index_of(vaddr, 2);
    uint64_t lv1 = index_of(vaddr, 1);

    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml(pagemap, lv5);
        pml3 = vmm_get_pml(pml4, lv4);
        pml2 = vmm_get_pml(pml3, lv3);
        pml1 = vmm_get_pml(pml2, lv2);
        pml1[lv1] = 0;
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml(pagemap, lv4);
        pml2 = vmm_get_pml(pml3, lv3);
        pml1 = vmm_get_pml(pml2, lv2);
        pml1[lv1] = 0;
    }
    invlpg(vaddr);
    release_lock(&vmm_lock);
}

void vmm_remap(uint64_t *pagemap, size_t vaddr_old, size_t vaddr_new, int flags)
{
    if (pagemap == NULL)
        pagemap = kernel_pagemap;

    acquire_lock(&vmm_lock);
    uint64_t lv4 = index_of(vaddr_old, 4);
    uint64_t lv3 = index_of(vaddr_old, 3);
    uint64_t lv2 = index_of(vaddr_old, 2);
    uint64_t lv1 = index_of(vaddr_old, 1);
    size_t paddr = 0;

    /* Save the physical address `vaddr_old` was mapped to, purge that entry in the TLB
       and remap the page to a new virtual address (physical address stays the same)
    */
    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr_old, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(kernel_pagemap, lv5, flags);
        pml3 = vmm_get_pml_or_alloc(pml4, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        paddr = pml1[lv1] & ~(511);
        pml1[lv1] = 0;
        invlpg(vaddr_old);
    }

    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(kernel_pagemap, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        paddr = pml1[lv1] & ~(511);
        pml1[lv1] = 0;
        invlpg(vaddr_old);
    }

    int pat_attr = get_pat();
    lv4 = index_of(vaddr_new, 4);
    lv3 = index_of(vaddr_new, 3);
    lv2 = index_of(vaddr_new, 2);
    lv1 = index_of(vaddr_new, 1);
    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr_new, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(kernel_pagemap, lv5, flags);
        paging_cache_disable_set(pml4, lv4, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml4, lv4, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml4, lv4, pat_attr & (1 << 12));

        pml3 = vmm_get_pml_or_alloc(pml4, lv4, flags);
        paging_cache_disable_set(pml3, lv3, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml3, lv3, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml3, lv3, pat_attr & (1 << 12));

        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        paging_cache_disable_set(pml2, lv2, pat_attr & PG_PCD);
        paging_write_through_set(pml2, lv2, pat_attr & PG_PWT);
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml2, lv2, pat_attr & (1 << 12));

        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);
        paging_cache_disable_set(pml1, lv1, pat_attr & PG_PCD);
        paging_write_through_set(pml1, lv1, pat_attr & PG_PWT);
        paging_pat_set(pml1, lv1, pat_attr & (1 << 7));
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;

        pml3 = vmm_get_pml_or_alloc(kernel_pagemap, lv4, flags);
        paging_cache_disable_set(pml3, lv3, pat_attr & (1 << PG_PCD));
        paging_write_through_set(pml3, lv3, pat_attr & (1 << PG_PWT));
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml3, lv3, pat_attr & (1 << 12));

        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        paging_cache_disable_set(pml2, lv2, pat_attr & PG_PCD);
        paging_write_through_set(pml2, lv2, pat_attr & PG_PWT);
        if (flags & (1 << 7)) // PS
            paging_pat_set(pml2, lv2, pat_attr & (1 << 12));

        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);
        paging_cache_disable_set(pml1, lv1, pat_attr & PG_PCD);
        paging_write_through_set(pml1, lv1, pat_attr & PG_PWT);
        paging_pat_set(pml1, lv1, pat_attr & (1 << 7));
    }

    invlpg(vaddr_new);
    release_lock(&vmm_lock);
}

void vmm_map_range(vmm_range_t range, int flags, uint64_t *pagemap)
{
    uint64_t base = range.range.base, limit = range.range.limit;

    for (; base < limit; base += PAGE_SIZE)
    {
        vmm_map(pagemap, base + range.address_offset, base, flags);
    }
}

void vmm_pagefault_handler(uint64_t cr2, int error_code)
{
    /* CPL = 3 */
    if (error_code & 4)
    {
        printk("vmm", "TODO: Userspace pagefault must be handled!\n");
        while (1)
            ;
    }

    /* Non present page */
    else if (!(error_code & 1))
        vmm_map(NULL, cr2, cr2, (error_code & 2) ? MAP_KERN : MAP_READONLY);

    /* Page fault generated by ab Instruction fetch, only when NX is enabled (Always will be because of limine) */
    else if (error_code & 16)
        ;
}

uint64_t *vmm_get_kernel_pagemap(void)
{
    return kernel_pagemap;
}

uint64_t *vmm_create_new_pagemap(void)
{
    return (uint64_t *)pmm_alloc();
}

void vmm_copy_kernel_mappings(task_t task)
{
    memcpy64(task.pagemap, (const uint64_t *)kernel_pagemap, 512 * 8);
}

void vmm_switch_pagemap(task_t task)
{
    panic_if(
        task.pagemap == NULL && task.task_type == TASK_DAEMON,
        "The kernel daemon task '%s' attempted to switch to a NULL pagemap!", task.descriptor);

    memcpy((uint8_t *)active_pagemap, (const uint8_t *)task.pagemap, sizeof(task.pagemap));
    wrcr3(task.pagemap);
}

void vmm_switch_to_kernel_pagemap(void)
{
    wrcr3(kernel_pagemap);
}