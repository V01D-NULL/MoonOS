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

struct usable_mem_range {
    uintptr_t base[40];
    uintptr_t limit[40];
};

#endif // MEMDEFS_H