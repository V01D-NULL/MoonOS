#define PR_MODULE "lapic"

#include "apic.h"
#include <amd64/moon.h>
#include <amd64/msr.h>
#include <printk.h>
#include <stddef.h>
#include <int/interrupts.h>
#include <hal/acpi/acpi.h>
#include <hal/time/sleep.h>
#include <mm/addr.h>
#include <panic.h>
#include <mm/cpu/CR.h>
#include <amd64/paging/paging.h>

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
void timer_tick(struct iframe *isr)
{
	// auto cr3 = cr_read(CR3);
    if (isr->cs == 0x43 && isr->isr_number == 32)
    {
        switch_to_kernel_pagemap();
        debug(true, "Userprocess triggered an exception %d\n", isr->isr_number);
        // return;
    }

    debug(true, "Tick %d 0x%x\n", isr->isr_number, isr->cs);
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

    lapic_base = apic.lapic_addr + $high_vma;
    lapic_enable();
    pr_info("lapic_base = %p (msrbase: %p)\n", lapic_base, rdmsr(IA32_APIC_BASE));

    if (!is_isr_registered(32))
        install_isr(32, (isr_t)&timer_tick);

    ticks_for_quantum = lapic_calibrate_timer(10); // Timer triggers INT 32 after 20 usec
    lapic_oneshot_timer(ticks_for_quantum);
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
    lapic_write(LAPIC_TIMER_INITIAL_COUNT, ~0U);

    usleep(usec);

    lapic_write(LAPIC_LVT_TIMER_REGISTER, LAPIC_TIMER_MASK);
    auto ticks = ~0U - lapic_read(LAPIC_TIMER_CURRENT_COUNT);

    lapic_write(LAPIC_LVT_TIMER_REGISTER, (0 << 16) | (0 << 17) | 32);
    return ticks;
}

void lapic_oneshot_timer(uint64_t usec)
{
    lapic_write(LAPIC_TIMER_INITIAL_COUNT, usec);
}
