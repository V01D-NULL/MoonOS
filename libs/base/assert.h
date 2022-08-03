#ifndef LIB_KERNEL_ASSERT_H
#define LIB_KERNEL_ASSERT_H

#include <base/base-types.h>

void __assert_impl(string_view file, uint64_t line, string_view func, string_view desc);
#define assert(x) { (x) ? (void)0 : __assert_impl(__FILE__, __LINE__, __func__, #x); }

#endif // LIB_KERNEL_ASSERT_H