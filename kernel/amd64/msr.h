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
	IA32_TSC_DEADLINE = 0x6E0,
	GS_BASE = 0xC0000101,
	PAT_MSR = 0x277,
	STAR = 0xC0000081,
	LSTAR = 0xC0000082,
	CSTAR = 0xC0000083,
	IA32_FMASK = 0xC0000084
};

// clang-format off
$struct(Msr,
{
	uint32_t eax;
	uint32_t edx;
});
// clang-format on

inline uint64_t rdmsr(uint32_t msr)
{
	uint32_t eax, edx = 0;
	asm volatile("rdmsr"
				 : "=a"(eax), "=d"(edx)
				 : "c"(msr)
				 : "memory");
	return concat64(eax, edx);
}

inline void wrmsr(uint32_t msr, uint64_t in)
{
	uint32_t eax = in;
	uint32_t edx = in >> 32;
	asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr)
				 : "memory");
}

inline uint64_t rdtsc(void)
{
	uint32_t eax, edx;
	asm volatile("rdtsc"
				 : "=a"(eax), "=d"(edx));
	return concat64(eax, edx);
}

#endif // MSR_H