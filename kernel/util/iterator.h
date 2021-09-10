#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdint.h>

struct iterator {
    uint64_t base;
    uint64_t limit;
    uint64_t iterator_index;
};

/* I_* = Iterator_* */
#define I_BASE(iter) iter.base
#define I_LIMIT(iter) iter.limit
#define I_INDEX(iter) iter.iterator_index
#define I_UNPACK(iter) (struct iterator) {.base = iter.base, .limit = iter.limit, .iterator_index = iter.iterator_index}
#define I_POPULATE(iter, low, hi) (iter.base = low, iter.limit = hi)
#define I_ITERATE(iter) for (iter.iterator_index = iter.base; iter.iterator_index < (iter.base + iter.limit); iter.iterator_index++)

#endif // ITERATOR_H