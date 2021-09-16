#include "x86.h"
#include <amd64/cpu.h>
#include <amd64/validity.h>
#include <drivers/vbe/vbe.h>
#include <drivers/io/serial.h>

__export cpu_hw_t cpu_hardware;

void x86_cpuid_vendor_string(const char *rax, const char *rbx, const char *rcx)
{
    char *first_half = strcat((char*)&rax, (char*)&rbx);
    char *final_string = strcat(first_half, (char*)&rcx);
    cpu_hardware.vendor_string = final_string;

    printk("cpuid", "CPU Vendor: %s\n", final_string);
    debug(false, "===========================================\nVendor string: %s\n===========================================\n", final_string);
}