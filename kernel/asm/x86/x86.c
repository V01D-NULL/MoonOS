#include "x86.h"

// void x86_cpu_info()
// {
//     ASM_x86_cpuid_vendor_string();
// }

void x86_cpuid_vendor_string(const char *a, const char *b, const char *c)
{
    //a, b and c are ebx, edx and ecx after executing cpuid. I am too lazy to fix the variables names so they will stay a, b, and c for now
    char* half = strcat((char*)&a, (char*)&b);
    const char* cpu_vendor = strcat(half, (char*)&c);
    kprintf("%s\n", cpu_vendor);

    debug("===========================================\nVendor string: %s\n===========================================\n", cpu_vendor);
}


void x86_cpu_check_apic()
{
    uint64_t rdx = ASM_x86_cpuid_check_bi_local_apic();
    
    debug("apic_support_rdx: %b\n", rdx);
}
