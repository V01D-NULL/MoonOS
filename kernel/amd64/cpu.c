#define PR_MODULE "cpu"

#include "cpu.h"
#include "msr.h"
#include <printk.h>
#include <mm/mm.h>
#include <mm/dynamic/kmalloc.h>
#include <hal/apic/apic.h>

void init_percpu(uint64_t current_stack)
{
    struct percpu *pcpu = (struct percpu *)kmalloc(sizeof(struct percpu), KMEM_PANIC | KMEM_HIGH_VMA);

    pcpu->syscall_stack = current_stack;
    pcpu->working_stack = 0; // Set by the syscall handler

    printk("pcpu", "0x%p | pcpu->syscall_stack: 0x%p\n", pcpu, pcpu->syscall_stack);
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
        : "a"(cpuid_regs->function));
}

// Return the apic id of the
// processor executing this code.
int current_cpu(void)
{
    // TODO: Add support for x2APIC mode.
    return lapic_read(0x20) >> 24;
}