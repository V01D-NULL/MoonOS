#ifndef PMM_H
#define PMM_H

/*
    Page Frame Allocation and alike
*/
#include "../common.h"
#include "../panic.h"
#include "../multiboot.h"
#include "../drivers/screen/monitor.h"

#define BITMAP_SZ 32768

#define PMM_FREE 1
#define PMM_USED 0
#define PMM_4KB  0x10000 //Also known as blocksize
#define FREE_OR_USED_BIT 5 //Bit 5 will determine whether the memoryblock is free or not (0 or 1)

//This will mainly be used in the bitmap array. It defines the value (free or used) at the given index
#define SET_FLAG(bit_val) (bit_val << FREE_OR_USED_BIT)

void pmm_init();
u32int pmm_alloc();
void pmm_free();
static void pmm_mark_as_used();

#endif // PMM_H