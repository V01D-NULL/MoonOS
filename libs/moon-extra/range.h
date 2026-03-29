#ifndef RANGE_H
#define RANGE_H

#include <base/base-types.h>

typedef struct
{
    size_t base;
    size_t limit;
} Range;

#define RANGE(x, y)           \
    (Range)                   \
    {                         \
        .base = x, .limit = y \
    }

#endif  // RANGE_H