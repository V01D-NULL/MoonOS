#include "apic.h"
#include <amd64/moon.h>
#include <amd64/msr.h>
#include <printk.h>

static uint32_t lapic_base = 0x0;

void lapic_init(struct apic_device_info apic)
{
    lapic_base = apic.lapic_addr;

    printk("lapic", "lapic base: %lX\n", apic.lapic_addr);

    for (int i = 0; i < apic.usable_ioapics; i++)
        printk("lapic", "I/O APIC base: %X\n", apic.ioapics[i]->address);
}

void lapic_read()
{

}