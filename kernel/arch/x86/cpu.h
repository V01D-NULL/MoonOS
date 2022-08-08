#ifndef CPU_H
#define CPU_H

#include "msr.h"
#include <base/base-types.h>

struct CpuidContext
{
	int eax;
	int ebx;
	int ecx;
	int edx;
	int function;
};

// cpu initialization (gs_base, stacks, lapics, etc)
void init_percpu(uint64_t current_stack);

void cpuid(struct CpuidContext *cpuid_context);
int current_cpu(void);

/**
 * A list of general purpose registers.
 */
struct$(GeneralRegisters, {
	int64_t r15;
	int64_t r14;
	int64_t r13;
	int64_t r12;
	int64_t r11;
	int64_t r10;
	int64_t r9;
	int64_t r8;
	int64_t rsi;
	int64_t rdi;
	int64_t rbp;
	int64_t rdx;
	int64_t rcx;
	int64_t rbx;
	int64_t rax;
});

struct Percpu
{
	uint64_t syscall_stack; // Temporary stack for the syscall handler.
	uint64_t working_stack; // Kernel stack
} PACKED;

// https://github.com/davxy/beeos/blob/abc3335738c06aac9a23ce3a292c91da8b46a460/kernel/src/mm/zone.c
// https://github.com/vercas/Beelzebub/blob/master/beelzebub/src/memory/kernel.vas.cpp
// https://download.freebsd.org/doc/en/books/design-44bsd/design-44bsd_en.pdf
// https://github.com/freebsd/freebsd-src
// https://wiki.osdev.org/Context_Switching
// https://en.wikipedia.org/wiki/Computer_multitasking
#define this_cpu                         \
	{                                    \
		(struct Percpu *)rdmsr(GS_BASE); \
	}

inline void arch_halt_cpu(void)
{
	for (;;)
		asm("hlt");
}

inline int64_t cr2_read(void)
{
	int64_t cr;
	asm volatile(
		"mov %%cr2, %0\n\t"
		: "=r"(cr)
		:
		: "%rax");
	return cr;
}

inline int64_t cr3_read(void)
{
	int64_t cr;
	asm volatile(
		"mov %%cr3, %0\n\t"
		: "=r"(cr)
		:
		: "%rax");
	return cr;
}

#endif // CPU_H