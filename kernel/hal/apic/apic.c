#include "apic.h"
#include <amd64/moon.h>
#include <amd64/msr.h>
#include <printk.h>

gnu_unused static uint32_t lapic_base = 0x0;

void lapic_init(void)
{
    msr_t regs;
    memset((void*)&regs, 0, sizeof(msr_t));

    rdmsr(IA32_APIC_BASE, regs);
    printk("lapic", "EAX:%X-EDX:%X\n", regs.eax, regs.edx);
}

void lapic_read()
{

}