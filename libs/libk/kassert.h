/**
 * @file kassert.h
 * @author Tim (V01D)
 * @brief Kernel assertion
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef LIB_KERNEL_ASSERT_H
#define LIB_KERNEL_ASSERT_H

#include <stdint.h>

void __assert_impl(const char *file, uint64_t line, const char* func, const char *desc);

/**
 * @brief Simple assertion function.
 * 
 * @param[in] x 'x' is a logical expression. It fails the assertion if x is not true, if x is true however, the assertion succeeds
 */
#define assert(x) ({ (x) ? 0 : __assert_impl(__FILE__, __LINE__, __FUNCTION__, #x); })



#endif // LIB_KERNEL_ASSERT_H