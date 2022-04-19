#include "cpu.h"
#include "msr.h"
#include <printk.h>
#include <mm/slab.h>
#include <mm/mm.h>

void init_percpu(uint64_t current_stack)
{
    slab_panic(true);
    struct percpu *pcpu = (struct percpu *)slab_alloc(sizeof(struct percpu));
    
    pcpu->syscall_stack = current_stack;
    pcpu->working_stack = 0; // Set by the syscall handler
    
    slab_panic(false);
    printk("percpu", "pcpu: 0x%p | pcpu->syscall_stack: 0x%p\n", pcpu, pcpu->syscall_stack);
    wrmsr(GS_BASE, (uint64_t)pcpu);
}

void log_cpuid_results(void)
{
    // The vendor string itself will do for now
    ASM_x86_cpuid_vendor_string();
}

void cpuid(struct cpuid_regs_t *cpuid_regs)
{
    asm(
        "cpuid"
        : "=a"(cpuid_regs->eax), "=b"(cpuid_regs->ebx), "=c"(cpuid_regs->ecx), "=d"(cpuid_regs->edx)
        : "a"(cpuid_regs->function)
    );
}