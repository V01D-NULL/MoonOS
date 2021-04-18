#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <liballoc/bitmap.h>
#include "ram.h"
#include <stdint.h>
#include "../drivers/vga/vga.h"

#define PMM_USED    1
#define PMM_FREE    0
#define PAGE_SIZE   (4096)

#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

void init_pmm(struct stivale2_mmap_entry *mmap, int entries);

void pmm_clear(uint8_t bit);
void pmm_set(uint8_t bit);

typedef struct pmm_bmp {
    uint64_t size;
    uint64_t base;
    uint64_t top;
} pmm_bitmap_t;

#endif // BITMAP_PMM_H