#ifndef MSR_H
#define MSR_H

#include "cpu.h"
#include "moon.h"
#include "bytes.h"
#include <ktypes.h>

#define IA32_APIC_BASE  0x1B
#define PAT_MSR         0x277

typedef struct
{
    uint32_t eax;
    uint32_t edx;
} msr_t;

STATIC_INLINE bool cpu_has_msr()
{
    /* The presence of MSRs on your processor is indicated by CPUID.01h:EDX[bit 5]. */
    struct cpuid_regs_t regs;
    regs.eax = 0x01;
    cpuid(&regs);
    return regs.edx & 1 << 5;
}

STATIC_INLINE uint64_t rdmsr(uint32_t msr)
{
    uint32_t eax, edx = 0;
    __asm__ volatile("rdmsr"
                     : "=a"(eax), "=d"(edx)
                     : "c"(msr)
                     : "memory");
    return concat64(eax, edx);
}

STATIC_INLINE void wrmsr(uint32_t msr, uint64_t in)
{
    uint32_t eax = in;
    uint32_t edx = in >> 32;
    __asm__ volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr) : "memory");
}

#endif // MSR_H