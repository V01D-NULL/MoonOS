/**
 * @file cpu.h
 * @author Tim (V01D)
 * @brief Manages anything directly CPU related
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef CPU_H
#define CPU_H

/*
    While the folder may say amd64/ this OS is fine when working with Intel CPU's aswell.
    I just refer to the AMD manual rather than the Intel manual and a cpu/ folder that doesn't hold gdt/idt seemed kinda lame so I went the AMD/
*/

#include <stdint.h>
#include <asm/x86/x86.h>
#include <boot/boot.h>

struct cpuid_regs_t
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

void cpuid(struct cpuid_regs_t *cpuid_regs);
void log_cpuid_results(void);

int current_cpu(void);

/**
 * @brief A list of registers preserved by the interupt stack frame
 * 
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
} gp_registers_t;

struct percpu
{
    uint64_t syscall_stack; // Temporary stack for the syscall handler.
    uint64_t working_stack; // Denotes the stack that is usually used when a cpu is not in a syscall or anything else that may require a stack change.
};

#endif // CPU_H