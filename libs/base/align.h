#ifndef BASE_ALIGN_H
#define BASE_ALIGN_H

#include <base/base-types.h>
#include <mm/page.h>
#include <stdalign.h>

#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

inline uint64_t align(int size, int align)
{
    return (size + align - 1) & ~(align - 1);
}

#endif  // BASE_ALIGN_H