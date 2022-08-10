#ifndef ARCH_H
#define ARCH_H

#include <base/base-types.h>

inline size_t arch_get_sp(void)
{
	size_t sp = 0;
	asm volatile("mov %0, sp"
				 : "=r"(sp));

	return sp;
}

inline size_t arch_get_bp(void)
{
	size_t bp = 0;
	asm volatile("mov %0, fp"
				 : "=r"(bp));

	return bp;
}

#endif // ARCH_H