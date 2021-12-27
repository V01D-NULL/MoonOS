#ifndef APIC_H
#define APIC_H

#include <ctypes.h>
#include <hal/acpi/tables/madt/madt.h>

void lapic_init(struct apic_device_info apic);

#endif // APIC_H