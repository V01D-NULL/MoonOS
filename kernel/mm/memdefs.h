#ifndef MEMDEFS_H
#define MEMDEFS_H

#include <stdint.h>
#include "pmm.h"
#include <util/ptr.h>
#include <mm/cpu/CR.h>

static const uintptr_t VMEM_LV5_BASE = 0xff00000000000000UL;
static const uintptr_t VMEM_LV4_BASE = 0xffff800000000000UL;
static const uintptr_t KERNEL_CODE_BASE = 0xffffffff80000000;

static inline bool check_la57() { return (cr_read(CR4) >> 12) & 1; }

/* These two helper utils were inspired by brutal */
static inline uintptr_t to_virt(uintptr_t phys)
{
    if (check_la57())
        return GENERIC_CAST(uintptr_t, phys + VMEM_LV5_BASE);

    return GENERIC_CAST(uintptr_t, phys + VMEM_LV4_BASE);
}
static inline uintptr_t to_phys(uintptr_t physical_addr)
{
    return GENERIC_CAST(uintptr_t, physical_addr + KERNEL_CODE_BASE);
}

static inline uintptr_t from_virt(uintptr_t phys)
{
    if (check_la57())
        return GENERIC_CAST(uintptr_t, phys - VMEM_LV5_BASE);
    
    return GENERIC_CAST(uintptr_t, phys - VMEM_LV4_BASE);
}

static inline uintptr_t from_phys_higher_half(uintptr_t virt)
{
    return GENERIC_CAST(uintptr_t, virt - KERNEL_CODE_BASE);
}

static inline bool is_page_aligned(void *addr) { return GENERIC_CAST(size_t, addr) % PAGE_SIZE == 0; }

static inline size_t get_page_count(void *addr, size_t n)
{
    size_t pagecount = 0;

    for (size_t i = 0; i < n; i += PAGE_SIZE)
    {
        if (is_page_aligned(addr))
        {
            pagecount++;
        }
    }

    return pagecount;
}

#endif // MEMDEFS_H
