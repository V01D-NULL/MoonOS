#ifndef PMM_PAGING_PFA_H
#define PMM_PAGING_PFA_H

#include <stdint.h>
#include "../pmm.h"
#include <liballoc/bitmap.h>

/*
    Page Frame Allocator
*/

//Todo: Implement this
void *request_page(void *address);

//Mark a single page as free
int pfa_mark_page_as_free(uint8_t *bitmap, void *address);

//Mark a single page as used
int pfa_mark_page_as_used(uint8_t *bitmap, void *address);

#endif // PMM_PAGING_PFA_H