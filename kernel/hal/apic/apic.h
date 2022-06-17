#ifndef APIC_H
#define APIC_H

#include <ktypes.h>
#include <hal/acpi/tables/madt/madt.h>

void lapic_init();
uint32_t lapic_read(uint32_t offset);
void lapic_write(uint32_t offset, uint32_t value);
void lapic_eoi(void);

void lapic_oneshot_timer(uint64_t usec);
uint32_t lapic_calibrate_timer(uint64_t usec);


#endif // APIC_H