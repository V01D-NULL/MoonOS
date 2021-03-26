#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <bits/stdint-uintn.h>
#include <liballoc/bitmap.h>
#include "ram.h"
#include <stdint.h>
#include "../drivers/vga/vga.h"

#define BITMAP_SIZE (calc_bitmap_size())

static inline uint64_t calc_bitmap_size()
{
    return ram_manager_get_total() / 0x1000 / 8;
}

void init_pmm();

typedef struct bitmap {
    uint64_t bitmap;
}bitmap_t;

#endif // BITMAP_PMM_H