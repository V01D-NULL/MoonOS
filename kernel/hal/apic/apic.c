#define PR_MODULE "lapic"

#include "apic.h"
#include <amd64/moon.h>
#include <amd64/msr.h>
#include <printk.h>
#include <stddef.h>
#include <stdalign.h>

static uint32_t lapic_base = 0x0;

void lapic_init(struct apic_device_info apic)
{
    lapic_base = apic.lapic_addr;

    pr_info("lapic base: %lX\n", apic.lapic_addr);

    for (int i = 0; i < apic.usable_ioapics; i++)
        pr_info("I/O APIC base: %X\n", apic.ioapics[i]->address);

    pr_info("%x\n", lapic_read(0x30));
}

uint32_t lapic_read(uint32_t offset)
{
    alignas(128) auto reg = (size_t*)((size_t)lapic_base + offset);
    return *(volatile uint32_t*)(reg);
}