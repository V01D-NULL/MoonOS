#ifndef PMM_H
#define PMM_H

/*
    Page Frame Allocation and alike
*/
#include "../common.h"
#include "../multiboot.h"
#include "../drivers/screen/monitor.h"

#define BITMAP_SZ 32768

#define PMM_FREE 1
#define PMM_USED 0

//Set a bit to bit_val at index idx
#define SET_FLAG(bit_val, idx) (bit_val << idx)

void pmm_init();
void pmm_alloc();
void pmm_free();

#endif // PMM_H