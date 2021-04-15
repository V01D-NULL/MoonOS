#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <liballoc/bitmap.h>
#include "ram.h"
#include <stdint.h>
#include "../drivers/vga/vga.h"

#define PMM_USED    0
#define PMM_FREE    1
#define PAGE_SIZE   4096

void init_pmm(struct stivale2_struct_tag_memmap *mmap, int entries);

void pmm_set(uint8_t bit);
void pmm_unset(uint8_t bit);

typedef struct pmm_bmp {
    uint64_t size;
    uint64_t base;
    uint64_t top;
} pmm_bitmap_t;

#endif // BITMAP_PMM_H