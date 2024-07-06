#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#include <base/base-types.h>
#include <boot/stivale2.h>
#include <moon-extra/range.h>
#include <moon.h>

#define arch_alloc_page() arch_alloc_page_sz(4096)

void *arch_alloc_page_sz(int sz);
void  arch_free_page(void *page);

#endif  // PHYS_ALLOC_H