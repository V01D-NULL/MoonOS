#ifndef PMM_PAGING_PFA_H
#define PMM_PAGING_PFA_H

#include <stdint.h>
#include "../pmm.h"
#include <liballoc/bitmap.h>

/*
    Page Frame Allocator
*/

/* Convert a bitmap bit index to an absolute address and vice versa */
#define BIT_TO_ADDRESS(x) (MM_BASE + (x * PAGE_SIZE * 8))
#define ADDRESS_TO_BIT(x) ((uint64_t)address / PAGE_SIZE)

//Todo: Implement this
void *request_pages(void *address);

//Mark a single page as free
int pfa_mark_page_as_free(void *address, bool calculate_absolute_address);

//Mark a single page as used
int pfa_mark_page_as_used(void *address, bool calculate_absolute_address);

#endif // PMM_PAGING_PFA_H
