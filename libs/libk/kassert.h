#ifndef LIB_KERNEL_ASSERT_H
#define LIB_KERNEL_ASSERT_H

#include <stdint.h>

void _assert(const char *file, uint64_t line, const char* func, const char *desc);

#define assert(x)  x ? (int) 0 : _assert(__FILE__, __LINE__, __FUNCTION__, #x)

#endif // LIB_KERNEL_ASSERT_H