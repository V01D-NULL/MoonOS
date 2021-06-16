#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <liballoc/bitmap.h>
#include "ram.h"
#include <stdint.h>
#include "../drivers/vga/vga.h"
#include "../amd64/validity.h"

#define PAGE_SIZE   (4096)
#define PMM_INVALID 0xc0ffee
#define PMM_FREE    0
#define PMM_USED    1
#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))
#define PMM_ABS_ADDRESS(address) ((0xffffffffUL << 32) | address)

__export size_t __offset__[];
#define MM_BASE (uintptr_t)__offset__

void pmm_init(struct stivale2_mmap_entry *mmap, int entries);

void *pmm_alloc();
int32_t pmm_free(void *page);
int32_t find_first_free_block();
void pmm_alloc_pages(uint64_t n);

typedef struct pmm
{
    liballoc_bitmap_t bitmap_manager;
    bitmap_size_type *bitmap; //Bitmap buffer / bitmap arena
    size_t size;
    bool is_bitmap_hosted; //If all attempts to host the bitmap fail this boolean will kick in and kernel panic
} pmm_t;

#endif // BITMAP_PMM_H
