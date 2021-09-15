#include "cpu.h"
#include <drivers/gfx/gfx.h>

cpu_hw_t cpu_hardware;

void cpu_info_init(boot_info_t cpu_info)
{   
    cpu_hardware.cpu_count = cpu_info.cpu.processor_count;
    cpu_hardware.bsp_lapic_id = cpu_info.cpu.bootstrap_processor_lapic_id;
    //Vendor string is set by asm/x86/cpuid.s
}

void cpu_info()
{
    ASM_x86_cpuid_vendor_string();
    printk("cpuid", "CPU Count: %d\n", cpu_hardware.cpu_count);
}

void cpuid(struct cpuid_regs_t *cpuid_regs)
{
    __asm__ volatile(
        __asm_flavor_intel
        "mov eax, %0\n"
        "cpuid\n"
        "mov %0, eax\n"
        "mov %1, ebx\n"
        "mov %2, ecx\n"
        "mov %3, edx\n"
        __asm_flavor_at_t
        : "=r"(cpuid_regs->eax), "=r"(cpuid_regs->ebx), "=r"(cpuid_regs->ecx), "=r"(cpuid_regs->edx)
        : "g"(cpuid_regs->function)
        : "eax", "ebx", "ecx", "edx"
    );
}