#include "syscalls.h"
#include <int/gdt.h>
#include <amd64/msr.h>
#include <printk.h>

extern void x86_syscall_handler(void);

void init_syscalls(void)
{
    wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)
    wrmsr(STAR, (KRNL_CS64 << 32) | (KRNL_DS64 | 3) << 48);
    wrmsr(LSTAR, (uint64_t)&x86_syscall_handler);
}

void syscall_handler(uint64_t current_stack)
{
    debug(true, "Syscall handler (stack: %llX)\n", current_stack);
}