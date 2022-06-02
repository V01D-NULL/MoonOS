#ifndef APIC_H
#define APIC_H

#include <ktypes.h>
#include <hal/acpi/tables/madt/madt.h>

void lapic_init(struct apic_device_info apic);
uint32_t lapic_read(uint32_t offset);

#endif // APIC_H