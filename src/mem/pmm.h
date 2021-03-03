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
#define GET_ALLOCATED_MEMORY(i, j) internal_get_allocated_memory(i, j)

static inline void *internal_get_allocated_memory(int i, int j)
{
  return (void*)((i * 32 + j) * 4096);
}

static inline u32int internal_pmm_idx(void *page)
{  
    return (u32int) page / 4096;
}

void pmm_init(struct multiboot *ptr);
void pmm_free(void *page);
void *pmm_alloc();
static void pmm_mark_as_used(void *page);
u64int total_ram(struct multiboot *ptr);

#endif // PMM_H
