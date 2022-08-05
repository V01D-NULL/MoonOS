#define PR_MODULE "cpu"

#include "cpu.h"
#include "msr.h"
#include <mm/mm.h>
#include <printk.h>
#include <platform/acpi/x86/apic/apic.h>
#include <mm/dynamic/kmalloc.h>
#include <platform/acpi/x86/hpet/hpet.h>
#include <platform/acpi/x86/acpi.h>

void init_percpu(uint64_t current_stack)
{
    struct percpu *pcpu = (struct percpu *)kmalloc(sizeof(struct percpu), KMEM_PANIC | KMEM_HIGH_VMA);

    // NOTE: Every logical core (which each calls init_percpu) shares it's stack with the syscall handler
    pcpu->syscall_stack = current_stack;
    pcpu->working_stack = 0; // Set by the syscall handler

    printk("pcpu", "0x%p | pcpu->syscall_stack: 0x%p\n", pcpu, pcpu->syscall_stack);
	wrmsr(IA32_KERNEL_GS_BASE, (uint64_t)pcpu);
    wrmsr(GS_BASE, (uint64_t)pcpu);

    lapic_init();
}

void cpuid(struct cpuid_context *cpuid_context)
{
    asm(
        "cpuid"
        : "=a"(cpuid_context->eax), "=b"(cpuid_context->ebx), "=c"(cpuid_context->ecx), "=d"(cpuid_context->edx)
        : "a"(cpuid_context->function));
}

// Return the apic id of the
// processor executing this code.
int current_cpu(void)
{
    // TODO: Add support for x2APIC mode.
    return lapic_read(0x20) >> 24;
}