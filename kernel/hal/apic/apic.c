#define PR_MODULE "lapic"

#include "apic.h"
#include <amd64/moon.h>
#include <amd64/msr.h>
#include <printk.h>
#include <stddef.h>
#include <int/interrupts.h>
#include <hal/acpi/acpi.h>
#include <hal/acpi/tables/hpet/hpet.h>
#include <panic.h>

static uint32_t lapic_base = 0x0;

enum
{
    LAPIC_LVT_TIMER_REGISTER = 0x320,
    LAPIC_TIMER_MASK = (0b1 << 16),
    LAPIC_TIMER_INITIAL_COUNT = 0x380,
    LAPIC_TIMER_CURRENT_COUNT = 0x390,
    LAPIC_TIMER_DIVIDE_CONFIG_REGISTER = 0x3E0
};

static int ticks_for_quantum = 0;

// Dummy isr for testing ig.
void timer_tick(isr_t isr)
{
    (void)isr;
    lapic_eoi();
    lapic_oneshot_timer(ticks_for_quantum);
}

static void lapic_enable(void)
{
    // Set spurious interrupt vector
    lapic_write(0xF0, lapic_read(0xF0) | (1 << 8) | 0xFF);
}

void lapic_init(void)
{
    struct apic_device_info apic;
    acpi_table_t madt;
    if ((madt = acpi_find_table("APIC")) != NULL)
        apic = madt_init(madt);
    else
        panic("No MADT table, no APIC - Refusing to boot");

    lapic_base = apic.lapic_addr;
    lapic_enable();

    if (!is_isr_registered(49))
        install_isr(49, &timer_tick);

    ticks_for_quantum = lapic_calibrate_timer(2000); // Timer triggers INT 49 after 20 usec
}

uint32_t lapic_read(uint32_t offset)
{
    auto reg = (size_t *)((size_t)lapic_base + offset);
    return *(volatile uint32_t *)(reg);
}

void lapic_write(uint32_t offset, uint32_t value)
{
    auto reg = (size_t *)((size_t)lapic_base + offset);
    *(volatile uint32_t *)(reg) = value;
}

void lapic_eoi(void)
{
    lapic_write(0xB0, 0);
}

// Convert 'usec' to ticks
uint32_t lapic_calibrate_timer(uint64_t usec)
{
    lapic_write(LAPIC_TIMER_DIVIDE_CONFIG_REGISTER, 3);
    lapic_write(LAPIC_TIMER_INITIAL_COUNT, ~0);

    hpet_usleep(usec);

    lapic_write(LAPIC_LVT_TIMER_REGISTER, LAPIC_TIMER_MASK);
    auto ticks = ~0 - lapic_read(LAPIC_TIMER_CURRENT_COUNT);

    lapic_write(LAPIC_LVT_TIMER_REGISTER, (0 << 16) | (0 << 17) | 49);
    return ticks;
}

void lapic_oneshot_timer(uint64_t usec)
{
    lapic_write(LAPIC_TIMER_INITIAL_COUNT, usec);
}