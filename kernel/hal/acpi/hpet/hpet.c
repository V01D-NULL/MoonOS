#define PR_MODULE "hpet"

#include "hpet.h"
#include <hal/acpi/acpi.h>
#include <printk.h>
#include <ktypes.h>
#include <mm/mm.h>
#include <panic.h>

#define HPET_ADDR_OFFSET 44 // 'address' offset in the hpet table. (Moon only uses the hpet to calibrate
                            // the lapic timer, so a HPET table struct seemed like overkill)

// Intel hpet spec 1-0a: 2.3.1 Register Overview
struct hpet_device // Represents a single register set for an hpet minus some things that I don't require for hpet_usleep()
{
    uint64_t general_capabilities;
    uint64_t reserved0;
    uint64_t general_configuration;
    uint64_t reserved1;
    uint64_t general_interrupt_status;
    uint64_t reserved2;
    uint64_t reserved3[24];
    uint64_t main_counter_value;
    uint64_t reserved4;
};

static volatile struct hpet_device *hpet_dev;

// Intel hpet spec 1-0a: General Capabilities and ID Register Bit Definitions
// This indicates the period at which
// the counter increments in femtoseconds (10^-15 seconds)
static int counter_tick_period;

void hpet_init(void)
{
    auto hpet = acpi_find_table("HPET");
    if (!hpet)
        panic("No HPET detected, cannot calibrate lapic timer!");

    auto hpet_addr = *(uint64_t *)(hpet + HPET_ADDR_OFFSET);

    hpet_dev = (struct hpet_device *)(hpet_addr);
    trace("HPET Address: %p\n", hpet_addr);

    counter_tick_period = (hpet_dev->general_capabilities >> 32);
    
    hpet_dev->general_configuration = 0;
    hpet_dev->main_counter_value = 0;
    hpet_dev->general_configuration = 1; // Enable the HPET
}

static uint64_t hpet_counter_value(void)
{
    return hpet_dev->main_counter_value;
}

// Converts a usec to the number of ticks the hpet will perform
const uint64_t hpet_usec_to_ticks(uint64_t usec)
{
    const uint64_t femtosecond = 1000000000;
    const uint64_t ticks = hpet_counter_value();
    return ticks + (usec * femtosecond) / counter_tick_period;
}

void hpet_usleep(uint64_t usec)
{
    const uint64_t required_hpet_ticks = hpet_usec_to_ticks(usec);
    while (hpet_counter_value() < required_hpet_ticks)
        asm volatile("pause" ::: "memory");
}