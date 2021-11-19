#include "heap.h"
#include <mm/pmm.h>
#include <devices/serial/serial.h>

// Todo: This needs to be an actual heap since
// physical pages can be non contigious in memory.
// Keep this for testing until an actual heap is implemented

heap_range_t heap_alloc(size_t bytes)
{
    size_t pages_to_alloc = ALIGN_UP(bytes) / PAGE_SIZE;
    return pmm_alloc_range(pages_to_alloc);
}

void heap_free(heap_range_t range)
{
    (void)range;
}