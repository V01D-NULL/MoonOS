#ifndef ADDR_H
#define ADDR_H

#include <base/base-types.h>
#include <moon.h>

// clang-format off
#define USER_REGION_BOTTOM 0x0000000000001000ULL  // 4 KiB because NULL is unmapped
#define USER_REGION_TOP 0x00007fffffffffffULL
#define CANONICAL_ADDR_MASK 0xffff000000000000ULL
// clang-format on

#define IS_CANONICAL(addr)                \
    ((addr & CANONICAL_ADDR_MASK) == 0 || \
     (addr & CANONICAL_ADDR_MASK) == CANONICAL_ADDR_MASK)

#define KiB(x) 1024 * x
#define MiB(x) (1024 * 1024) * x
#define GiB(x) (1024LL * 1024LL * 1024LL) * x

typedef uint64_t virt_t, phys_t;

void    set_hhdm_offset(size_t offset);
phys_t *va(phys_t paddr);
virt_t *pa(virt_t vaddr);

bool is_user_address(virt_t addr);

#endif  // ADDR_H