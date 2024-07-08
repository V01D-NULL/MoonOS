#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#include <base/base-types.h>
#include <moon-extra/range.h>
#include <moon-sys/handover.h>
#include <moon.h>

#define arch_alloc_page() arch_alloc_page_sz(4096)

void  init_phys_allocator(HandoverMemoryMap mmap);
void *arch_alloc_page_sz(UNUSED int sz);
void  arch_free_page(UNUSED void *page);

#endif  // PHYS_ALLOC_H