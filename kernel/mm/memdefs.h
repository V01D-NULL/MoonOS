#ifndef MEMDEFS_H
#define MEMDEFS_H

#include <stdint.h>
#include "pmm.h"
#include "../util/ptr.h"

/* These two helper utils were inspired by brutal */
static inline uintptr_t to_virt(uintptr_t phys) { return GENERIC_CAST(uintptr_t, phys + VMM_BASE); }
static inline uintptr_t to_phys(uintptr_t virt) { return GENERIC_CAST(uintptr_t, virt +  MM_BASE); }

static inline uintptr_t from_virt(uintptr_t phys) { return GENERIC_CAST(uintptr_t, phys - VMM_BASE); }
static inline uintptr_t from_phys(uintptr_t virt) { return GENERIC_CAST(uintptr_t, virt -  MM_BASE); }

static inline bool is_page_aligned(void *addr) { return GENERIC_CAST(size_t, addr) % PAGE_SIZE == 0; }

static inline size_t get_page_count(void *addr, size_t n)
{
    size_t pagecount = 0;

    for (size_t i = 0; i < n; i+=PAGE_SIZE)
    {
        if (is_page_aligned(addr))
        {
            pagecount++;
        }
    }

    return pagecount;
}

#endif // MEMDEFS_H