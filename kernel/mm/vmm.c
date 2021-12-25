#include "vmm.h"
#include "memdefs.h"
#include <libk/kassert.h>
#include <amd64/moon.h>
#include <int/gdt.h>
#include <int/idt.h>
#include <util/ptr.h>
#include <stdbool.h>
#include <trace/sym.h>
#include <printk.h>
#include <panic.h>
#include <sys/smp/spinlock.h>

create_lock("vmm", vmm_lock);

static uint64_t *kernel_pagemap;
static bool la57_enabled = false;
static bool panic_on_remap = false;

STATIC_INLINE uint64_t index_of(uint64_t vaddr, int offset)
{
    return vaddr >> (12 + 9 * (offset-1)) & 0x1FF;
}

void vmm_should_panic(bool b)
{
    panic_on_remap = b;
}

void vmm_init(bool has_5_level_paging, struct stivale2_struct_tag_memmap *mmap)
{
    la57_enabled = has_5_level_paging;

    kernel_pagemap = (uint64_t *)(from_higher_half((uintptr_t)pmm_alloc(), DATA));

    if (la57_enabled) {
        debug(true, "Using 5 level paging\n");
    }
    
    // This mapping is required in order to use the stivale2 terminal
    for (size_t i = 0; i < mmap->entries; i++) 
    {
        size_t base = mmap->memmap[i].base;
        size_t top = base + mmap->memmap[i].length;
        size_t type = mmap->memmap[i].type;
        
        if (
            type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE ||
            type == STIVALE2_MMAP_FRAMEBUFFER
        )
        {
            vmm_map_range((range_t) {.base = base, .limit = top}, 0, PG_PR);
        }
    }

    // Map 2GiB of kernel data
    vmm_map_range((range_t){.base = 0, .limit = 2 * GB}, la57_enabled ? VMEM_LV5_BASE : VMEM_LV4_BASE, PG_RW);
    
    // Map 2GiB of kernel code
    vmm_map_range((range_t){.base = 0, .limit = 2 * GB}, VMEM_CODE_BASE, PG_RW);

    debug(true, "Old PML4: %llx\n", cr_read(CR3)); // Bootloader pagemap
    wrcr3(GENERIC_CAST(uint64_t, kernel_pagemap));
    debug(true, "New PML4: %llx\n", cr_read(CR3)); // Kernel pagemap
    vmm_should_panic(true);
}

static uint64_t *vmm_get_pml_or_alloc(uint64_t *entry, size_t level, int flags)
{
    if (entry[level] & 1)
        goto no_alloc;

    void *addr = NULL;
    assert((addr = pmm_alloc()) != NULL);
    entry[level] = from_higher_half(GENERIC_CAST(uintptr_t, addr), DATA);
    entry[level] |= flags;

no_alloc:
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

static uint64_t *vmm_get_pml(uint64_t *entry, size_t level)
{
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

void vmm_map(size_t vaddr, size_t paddr, int flags)
{
    acquire_lock(&vmm_lock);
    uint64_t lv4 = index_of(vaddr, 4);
    uint64_t lv3 = index_of(vaddr, 3);
    uint64_t lv2 = index_of(vaddr, 2);
    uint64_t lv1 = index_of(vaddr, 1);

    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(kernel_pagemap, lv5, flags);
        pml3 = vmm_get_pml_or_alloc(pml4, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);

        if(pml1[lv1] != 0 && panic_on_remap)
        {
            panic("Attempted to map a mapped page! [ virt: 0x%lX | phys: 0x%lX | pml1[%d]: 0x%lX ]",
                vaddr, paddr, lv1, pml1[lv1]
            );
        }

        pml1[lv1] = (paddr | flags);
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(kernel_pagemap, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        
        if (pml1[lv1] != 0 && panic_on_remap)
        {
            panic("Attempted to map a mapped page! [ virt: 0x%lX | phys: 0x%lX | pml1[%d]: 0x%lX ]",
                vaddr, paddr, lv1, pml1[lv1]
            );
        }

        pml1[lv1] = (paddr | flags);
    }
    release_lock(&vmm_lock);
}

void vmm_unmap(size_t vaddr)
{
    acquire_lock(&vmm_lock);
    uint64_t lv4 = index_of(vaddr, 4);
    uint64_t lv3 = index_of(vaddr, 3);
    uint64_t lv2 = index_of(vaddr, 2);
    uint64_t lv1 = index_of(vaddr, 1);

    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml(kernel_pagemap, lv5);
        pml3 = vmm_get_pml(pml4, lv4);
        pml2 = vmm_get_pml(pml3, lv3);
        pml1 = vmm_get_pml(pml2, lv2);
        pml1[lv1] = 0;
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml(kernel_pagemap, lv4);
        pml2 = vmm_get_pml(pml3, lv3);
        pml1 = vmm_get_pml(pml2, lv2);
        pml1[lv1] = 0;
    }
    invlpg(vaddr);
    acquire_lock(&vmm_lock);
}

void vmm_remap(size_t vaddr_old, size_t vaddr_new, int flags)
{
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

    lv4 = index_of(vaddr_new, 4);
    lv3 = index_of(vaddr_new, 3);
    lv2 = index_of(vaddr_new, 2);
    lv1 = index_of(vaddr_new, 1);
    if (la57_enabled)
    {
        uint64_t lv5 = index_of(vaddr_new, 5);
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(kernel_pagemap, lv5, flags);
        pml3 = vmm_get_pml_or_alloc(pml4, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);
        invlpg(vaddr_new);
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(kernel_pagemap, lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, lv2, flags);
        pml1[lv1] = (paddr | flags);
        invlpg(vaddr_new);
    }

    release_lock(&vmm_lock);
}

void vmm_map_range(range_t range, size_t offset, int flags)
{
    for (; range.base < range.limit; range.base += PAGE_SIZE)
    {
        vmm_map(range.base + offset, range.base, flags);
    }
}

//Pagefault handler
void vmm_guess_and_map(uint64_t cr2, int error_code)
{
    /* Non present page */
    if (error_code & 1)
        vmm_map(cr2, cr2, (error_code & 2) ? (PG_PR | PG_RW) : PG_PR);

    /* CPL = 3 */
    else if (error_code & 4)
        ;

    /* Page fault generated by ab Instruction fetch, only when NX is enabled (Always will be because of limine) */
    else if (error_code & 16)
        ;

    else
        vmm_map(cr2, cr2, PG_PR | PG_RW);
}

uint64_t *vmm_get_kernel_pagemap(void)
{
    return kernel_pagemap;
}

uint64_t *vmm_create_new_pagemap(void)
{
    return (uint64_t*)from_higher_half((uintptr_t)pmm_alloc(), DATA);
}