#ifndef KERNEL_HEAP_H
#define KERNEL_HEAP_H

#include <stdint.h>
#include <util/range.h>

typedef range_t heap_range_t;
heap_range_t heap_alloc(size_t bytes);
void heap_free(heap_range_t range);

#endif // KERNEL_HEAP_H