#ifndef MSR_H
#define MSR_H

#include "cpu.h"
#include "moon.h"
#include "bytes.h"
#include <ktypes.h>

enum
{
    IA32_APIC_BASE = 0x1B,
    IA32_EFER = 0xC0000080,
    IA32_KERNEL_GS_BASE = 0xC0000102,
    GS_BASE = 0xC0000101,
    PAT_MSR = 0x277,
    STAR = 0xC0000081,
    LSTAR = 0xC0000082,
    CSTAR = 0xC0000083
};

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
    return regs.edx & (1 << 5);
}

STATIC_INLINE uint64_t rdmsr(uint32_t msr)
{
    uint32_t eax, edx = 0;
    asm volatile("rdmsr"
                     : "=a"(eax), "=d"(edx)
                     : "c"(msr)
                     : "memory");
    return concat64(eax, edx);
}

STATIC_INLINE void wrmsr(uint32_t msr, uint64_t in)
{
    uint32_t eax = in;
    uint32_t edx = in >> 32;
    asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr)
                     : "memory");
}

#endif // MSR_H