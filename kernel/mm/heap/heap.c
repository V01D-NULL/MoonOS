#include "heap.h"
#include <mm/pmm.h>
#include <devices/serial/serial.h>

heap_range_t heap_alloc(size_t bytes)
{
    size_t pages_to_alloc = ALIGN_UP(bytes) / PAGE_SIZE;
    return pmm_alloc_range(pages_to_alloc);
}

void heap_free(heap_range_t range)
{
    (void)range;
}