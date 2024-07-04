#ifndef ADDR_H
#define ADDR_H

#include <base/base-types.h>
#include <moon.h>

#define KiB(x) 1024 * x
#define MiB(x) (1024 * 1024) * x
#define GiB(x) (1024LL * 1024LL * 1024LL) * x

typedef uint64_t virt_t, phys_t;

void    set_hhdm_offset(size_t offset);
phys_t *va(phys_t paddr);
virt_t *pa(virt_t vaddr);

#endif  // ADDR_H