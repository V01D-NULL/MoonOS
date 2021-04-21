#include "x86.h"
#include "../../amd64/cpu.h"
#include "../../drivers/gfx/gfx.h"
#include "../../drivers/io/serial.h"

extern cpu_hw_t cpu_hardware;

void x86_cpu_info()
{
    ASM_x86_cpuid_vendor_string();
    printk("cpuid", "CPU Count: %d\n", cpu_hardware.cpu_count);
}

void x86_cpuid_vendor_string(const char *rax, const char *rbx, const char *rcx)
{
    char *first_half = strcat((char*)&rax, (char*)&rbx);
    char *final_string = strcat(first_half, (char*)&rcx);
    cpu_hardware.vendor_string = final_string;

    printk("cpuid", "CPU Vendor: %s\n", final_string);
    debug("===========================================\nVendor string: %s\n===========================================\n", final_string);
}