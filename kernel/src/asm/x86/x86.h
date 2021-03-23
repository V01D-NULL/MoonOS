#ifndef ASM_ARCH_X86_H
#define ASM_ARCH_X86_H

/*
    x86.h - Takes all the assembly functions and provides them in a single header file for ease-of-access
*/

#include "../../drivers/screen/monitor.h"
#include "../../common.h"

//Assembly labels (must be prefixed with ASM_<ARCHITECTURE>):
extern const char *ASM_x86_cpuid_vendor_string();
extern uint64_t    ASM_x86_cpuid_check_apic   ();

//C functions:
void x86_cpu_info();
void x86_cpu_check_apic();

#endif // ASM_ARCH_X86_H