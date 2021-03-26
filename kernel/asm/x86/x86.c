#include "x86.h"

// void x86_cpu_info()
// {
//     ASM_x86_cpuid_vendor_string();
// }

void x86_cpuid_vendor_string(const char *rax, const char *rbx, const char *rcx)
{
    //a, b and c are ebx, edx and ecx after executing cpuid. I am too lazy to fix the variables names so they will stay a, b, and c for now
    
    kprintf_p(false, true, "CPU Vendor: %s%s%s\n", &rax, &rbx, &rcx);

    debug("===========================================\nVendor string: %s%s%s\n===========================================\n", &rax, &rbx, &rcx);
}
