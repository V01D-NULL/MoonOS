#ifndef PMM_H
#define PMM_H

/*
    Page Frame Allocation and alike
*/
#include "../common.h"
#include "../multiboot.h"

#define BITMAP_SZ 32768

//Set a bit to bit_val at index idx
#define SET_FLAG(bit_val, idx) (bit_val << idx)

void pmm_init();

#endif // PMM_H