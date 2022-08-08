#ifndef CPU_H
#define CPU_H

#include <base/base-types.h>
#include <moon.h>

int current_cpu(void);

/**
 * A list of general purpose registers.
 */
struct$(GeneralRegisters, {
	// int64_t r15;
	// int64_t r14;
	// int64_t r13;
	// int64_t r12;
	// int64_t r11;
	// int64_t r10;
	// int64_t r9;
	// int64_t r8;
	// int64_t rsi;
	// int64_t rdi;
	// int64_t rbp;
	// int64_t rdx;
	// int64_t rcx;
	// int64_t rbx;
	// int64_t rax;
});

struct Percpu
{
	uint64_t syscall_stack; // Temporary stack for the syscall handler.
	uint64_t working_stack; // Kernel stack
} PACKED;

inline void arch_halt_cpu(void)
{
	for (;;)
		asm("wfi");
}

#endif // CPU_H