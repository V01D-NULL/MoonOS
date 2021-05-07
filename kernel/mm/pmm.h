#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <liballoc/bitmap.h>
#include "ram.h"
#include <stdint.h>
#include "../drivers/vga/vga.h"

#define PAGE_SIZE   (4096)
#define PMM_INVALID 0xc0ffee
#define PMM_FREE    0
#define PMM_USED    1
#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

void init_pmm(struct stivale2_mmap_entry *mmap, int entries);

int32_t pmm_alloc();
int32_t pmm_free(uint8_t bit_index);
int32_t find_first_free_block();

typedef struct pmm
{
    size_t phys_start;
    liballoc_bitmap_t bitmap_manager;
    bitmap_size_type *bitmap; //Bitmap buffer / bitmap arena
    size_t size;
    size_t *next;
} pmm_t;

#endif // BITMAP_PMM_H