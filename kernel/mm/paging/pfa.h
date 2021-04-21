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
int pfa_mark_page_as_free(void *address);

//Mark a single page as used
int pfa_mark_page_as_used(void *address);

#endif // PMM_PAGING_PFA_H
