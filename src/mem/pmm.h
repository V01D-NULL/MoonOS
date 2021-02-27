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
#define PMM_4KB  0x1000 //Also known as blocksize
#define PMM_IDX(page) internal_pmm_idx(page)

static inline u32int internal_pmm_idx(void *page)
{  
    return (u32int) page / 4096;
}

void pmm_init();
u32int pmm_alloc();
void pmm_free(void *page);
static void pmm_mark_as_used(void *page);

#endif // PMM_H