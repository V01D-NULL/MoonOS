#ifndef CPU_H
#define CPU_H

#include <boot/boot.h>
#include <ktypes.h>

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
$struct(general_registers, {
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
} PACKED;

#endif // CPU_H