#include "x86.h"
#include "../../AMD/cpu.h"

// void x86_cpu_info()
// {
//     ASM_x86_cpuid_vendor_string();
// }

void x86_cpuid_vendor_string(const char *rax, const char *rbx, const char *rcx)
{
    char *first_half = strcat((char*)&rax, (char*)&rbx);
    char *final_string = strcat(first_half, (char*)&rcx);
    cpu_hardware.vendor_string = final_string;

    kprintf_p(false, true, "CPU Vendor: %s\n", final_string);
    debug("===========================================\nVendor string: %s\n===========================================\n", final_string);
}