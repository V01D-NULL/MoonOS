#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <boot/boot.h>

struct cpuid_context
{
    int eax;
    int ebx;
    int ecx;
    int edx;
    int function;
};

// Todo: When smp is in question I'll make a cpu_ctx_t and pass that as an argument to init_percpu().
// cpu_ctx_t will probably just derive from the stivale2 smp tag or something
void init_percpu(uint64_t current_stack); // Initialize information for this CPU and store it in gs

void cpuid(struct cpuid_context *cpuid_context);
int current_cpu(void);

/**
 * A list of general purpose registers.
 */
typedef struct regs {
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
} general_registers;

struct percpu
{
    uint64_t syscall_stack; // Temporary stack for the syscall handler.
    uint64_t working_stack; // Denotes the stack that is usually used when a cpu is not in a syscall or anything else that may require a stack change.
};

#endif // CPU_H