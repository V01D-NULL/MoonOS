#include "cpuid.h"
#include "gnu-cpuid.h" //__get_cpuid, etc.. (Provided by GNU)

/*
Useful articles:
    https://software.intel.com/sites/default/files/managed/ba/f1/intel-64-architecture-processor-topology-enumeration.pdf
    https://en.wikipedia.org/wiki/CPUID
    https://wiki.osdev.org/Detecting_CPU_Speed
    http://bochs.sourceforge.net/techspec/24161821.pdf
    https://software.intel.com/content/www/us/en/develop/articles/intel-64-architecture-processor-topology-enumeration.html
    https://www.felixcloutier.com/x86/cpuid
    
*/

/* The cpu vendor strings split up into 3 registers. 4 chars each - File: cpuid-asm.s */
extern char* cpu_vendor_string(); // EBX, EDX, ECX

int get_cpu_model_number(void)
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    return ebx; //Model number
}

char* get_vendor_string()
{
    /*
        get_vendor_string ~ Get the CPU vendor string

        @return const char*
        @param none
        --
        = Get the vendor string
    */

    //CPU vendor string
    return cpu_vendor_string();
    
}

void get_cpu_brand_string()
{
    /*
        get_cpu_brand_string ~ Get the brand string of the CPU

        @return void
        @param none
        --
        = Use the cpuid.h lib provided by the GCC team to get the string
    */
    u32int brand[12];
    
    __get_cpuid(0x80000002, brand+0x0, brand+0x1, brand+0x2, brand+0x3);
    __get_cpuid(0x80000003, brand+0x4, brand+0x5, brand+0x6, brand+0x7);
    __get_cpuid(0x80000004, brand+0x8, brand+0x9, brand+0xa, brand+0xb);
    monitor_write((char*)brand, false, true);
}

void checkCPU() {
    /*
        checkCPU ~ Calls all CPU functions from this source file, neatly packed into a single function
        @return void
        @param none
        --
        = Get the CPU model number
        = Get the Vendor string
        = Get the CPU vendor (AMD, INTEL, etc..)
    */
    monitor_write("Checking the CPU\n", true, false);
    monitor_write("- MODEL NUMBER - VENDOR STRING - CPU BRAND STRING\n", false, false);
    
    op_ok();
    monitor_write_dec(get_cpu_model_number());
    monitor_writeln();
    monitor_write(get_vendor_string(), false, true);
    monitor_writeln();
    get_cpu_brand_string();
    monitor_writeln();
}
