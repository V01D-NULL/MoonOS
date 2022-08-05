#ifndef CPU_H
#define CPU_H

#include "msr.h"
#include <boot/boot.h>
#include <base/base-types.h>

struct cpuid_context
{
	int eax;
	int ebx;
	int ecx;
	int edx;
	int function;
};

// cpu-ish initialization (gs_base, stacks, lapics, etc)
void init_percpu(uint64_t current_stack);

void cpuid(struct cpuid_context *cpuid_context);
int current_cpu(void);

/**
 * A list of general purpose registers.
 */
struct$(general_registers, {
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

struct percpu
{
	uint64_t syscall_stack; // Temporary stack for the syscall handler.
	uint64_t working_stack; // Kernel stack
							// Task active_task;
} PACKED;

// https://github.com/davxy/beeos/blob/abc3335738c06aac9a23ce3a292c91da8b46a460/kernel/src/mm/zone.c
// https://github.com/vercas/Beelzebub/blob/master/beelzebub/src/memory/kernel.vas.cpp
// https://download.freebsd.org/doc/en/books/design-44bsd/design-44bsd_en.pdf
// https://github.com/freebsd/freebsd-src
// https://wiki.osdev.org/Context_Switching
// https://en.wikipedia.org/wiki/Computer_multitasking
#define this_cpu                         \
	{                                    \
		(struct percpu *)rdmsr(GS_BASE); \
	}

#endif // CPU_H