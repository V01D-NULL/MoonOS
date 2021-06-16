#ifndef PMM_PAGING_PFA_H
#define PMM_PAGING_PFA_H

#include <stdint.h>
#include "../pmm.h"
#include <liballoc/bitmap.h>

/*
    Page Frame Allocator
*/

/* Convert a bitmap bit index to an absolute address and vice versa */
#define BIT_TO_ADDRESS(x) (MM_BASE + (x * PAGE_SIZE * 8)) //x * PAGE_SIZE * 8 gets the amount of bytes in total, then we just add the base of the memory map define in MM_BASE and we have the relative address of the bit in the bitmap!
#define ADDRESS_TO_BIT(x) ((uint64_t)x / PAGE_SIZE / 8) //Here we do the same as above but in reverse (Note: You have to do x -= MM_BASE before calling this function. I'm to lazy to fix it so I wrote a lenghty comment instead)

#define PFA_ERR 0x1
#define PFA_OK     0x0

//Todo: Implement this
void *request_pages(void *address);

//Mark a single page as free
int pfa_mark_page_as_free(void *address, bool calculate_absolute_address);

//Mark a single page as used
int pfa_mark_page_as_used(void *address, bool calculate_absolute_address);

#endif // PMM_PAGING_PFA_H
