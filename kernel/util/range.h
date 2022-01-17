#ifndef RANGE_H
#define RANGE_H

#include <stddef.h>

typedef struct {
    size_t base;
    size_t limit;
} range_t;

#endif // RANGE_H