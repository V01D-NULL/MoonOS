#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#include <moon-extra/range.h>
#include <base/base-types.h>
#include <moon.h>

void *arch_alloc_page(void);
void arch_free_page(void *page);
Range arch_alloc_page_range(size_t pages);

#endif // PHYS_ALLOC_H