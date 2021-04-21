#include "cpu.h"

cpu_hw_t cpu_hardware;

void cpu_info_init(boot_info_t cpu_info)
{   
    cpu_hardware.cpu_count = cpu_info.cpu.processor_count;
    cpu_hardware.bsp_lapic_id = cpu_info.cpu.bootstrap_processor_lapic_id;
    //Vendor string is set by asm/x86/cpuid.s
}