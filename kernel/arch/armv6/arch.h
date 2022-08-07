#ifndef ARCH_H
#define ARCH_H

#include <base/base-types.h>

inline size_t arch_get_sp(void)
{
	return 0;
	// uint64_t rsp = 0;
	// asm volatile("mov %%rsp, %0"
	// 			 : "=r"(rsp));

	// return rsp;
}

inline size_t arch_get_bp(void)
{
	return 0;
	// uint64_t rbp = 0;
	// asm volatile("mov %%rbp, %0"
	// 			 : "=r"(rbp));

	// return rbp;
}

#endif // ARCH_H