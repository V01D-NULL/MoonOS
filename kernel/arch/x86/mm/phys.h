#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#include <base/base-types.h>
#include <moon.h>
#include <boot/stivale2.h>
#include <moon-extra/range.h>

void *arch_alloc_page(void);
void arch_free_page(void *page);
Range arch_alloc_page_range(size_t pages);

#endif // PHYS_ALLOC_H