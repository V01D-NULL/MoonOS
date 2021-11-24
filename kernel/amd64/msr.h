#ifndef MSR_H
#define MSR_H

#include "cpu.h"
#include "moon.h"
#include <ctypes.h>

#define IA32_APIC_BASE 0x1B

typedef struct
{
    uint32_t *eax;
    uint32_t *edx;
} msr_t;

STATIC_INLINE bool cpu_has_msr()
{
    /* The presence of MSRs on your processor is indicated by CPUID.01h:EDX[bit 5]. */
    struct cpuid_regs_t regs;
    regs.eax = 0x01;
    cpuid(&regs);
    return regs.edx & 1 << 5;
}

STATIC_INLINE void rdmsr(uint32_t msr, msr_t msr_registers)
{
    __asm__ volatile("rdmsr"
                     : "=a"(*msr_registers.eax), "=d"(*msr_registers.edx)
                     : "c"(msr));
}

STATIC_INLINE void wrmsr(uint32_t msr, msr_t msr_registers)
{
    __asm__ volatile("wrmsr" ::"a"(*msr_registers.eax), "d"(*msr_registers.edx), "c"(msr));
}

#endif // MSR_H