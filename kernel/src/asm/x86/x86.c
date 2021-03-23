#include "x86.h"

void x86_cpu_info()
{
    const char *cpu_vendor = ASM_x86_cpuid_vendor_string();
    kprintf("===========================================\nVendor string: %s\n===========================================\n", cpu_vendor);
}

void x86_cpu_check_apic()
{
    uint64_t rdx = ASM_x86_cpuid_check_apic();
    
    kprintf("apic_support_edx: %b\n", LOWER_32(rdx));
}