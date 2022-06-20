#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <stdint.h>
#include <amd64/moon.h>
#include <stivale2.h>
#include <stdbool.h>
#include <util/range.h>

#define PMM_FREE 0
#define PMM_USED 1

void pmm_init(struct stivale2_mmap_entry *mmap, int entries);
void *pmm_alloc(void);
void pmm_free(void *page);
range_t pmm_alloc_range(size_t pages);

#endif // BITMAP_PMM_H
