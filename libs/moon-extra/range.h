#ifndef RANGE_H
#define RANGE_H

#include <base/base-types.h>

struct$(Range, {
	size_t base;
	size_t limit;
});

#define RANGE(x, y) \
	(Range) { .base = x, .limit = y }

#endif // RANGE_H