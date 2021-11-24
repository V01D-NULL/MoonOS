#include "cpu.h"
#include <printk.h>

void log_cpuid_results(void)
{
    // The vendor string itself will do for now
    ASM_x86_cpuid_vendor_string();
}

void cpuid(struct cpuid_regs_t *cpuid_regs)
{
    __asm__ volatile(
        gnu_asm_flavor_intel
        "mov eax, %0\n"
        "cpuid\n"
        "mov %0, eax\n"
        "mov %1, ebx\n"
        "mov %2, ecx\n"
        "mov %3, edx\n"
        gnu_asm_flavor_at_t
        : "=r"(cpuid_regs->eax), "=r"(cpuid_regs->ebx), "=r"(cpuid_regs->ecx), "=r"(cpuid_regs->edx)
        : "g"(cpuid_regs->function)
        : "eax", "ebx", "ecx", "edx"
    );
}