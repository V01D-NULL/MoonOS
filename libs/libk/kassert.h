#ifndef LIB_KERNEL_ASSERT_H
#define LIB_KERNEL_ASSERT_H

#include <stdint.h>

void __assert_impl(const char *file, uint64_t line, const char* func, const char *desc);
#define assert(x) { (x) ? (void)0 : __assert_impl(__FILE__, __LINE__, __func__, #x); }

#endif // LIB_KERNEL_ASSERT_H