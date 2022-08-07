#define PR_MODULE "lapic"

#include "apic.h"
#include <moon.h>
#include <cpu.h>
#include <printk.h>
#include <base/base-types.h>
#include <interrupts.h>
#include <platform/acpi/x86/acpi.h>
#include <moon-sys/time/sleep.h>
#include <mm/addr.h>
#include <panic.h>
#include <paging/paging.h>

static uint64_t lapic_base = 0x0;

enum
{
	LAPIC_LVT_TIMER_REGISTER = 0x320,
	LAPIC_TIMER_MASK = (0b1 << 16),
	LAPIC_TIMER_INITIAL_COUNT = 0x380,
	LAPIC_TIMER_CURRENT_COUNT = 0x390,
	LAPIC_TIMER_DIVIDE_CONFIG_REGISTER = 0x3E0
};

static void lapic_enable(void)
{
	// Set spurious interrupt vector
	lapic_write(0xF0, lapic_read(0xF0) | (1 << 8) | 0xFF);
}

void lapic_init(void)
{
	struct apic_device_info apic;
	AcpiTable madt;
	if ((madt = acpi_find_table("APIC")) != NULL)
		apic = madt_init(madt);
	else
		panic("No MADT table, no APIC - Refusing to boot");

	lapic_base = apic.lapic_addr + $high_vma;
	lapic_enable();
	trace("lapic_base = %p\n", lapic_base);
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

int lapic_cpu_id(void)
{
	if (lapic_base)
		return lapic_read(0x20) >> 24;

	return 0;
}