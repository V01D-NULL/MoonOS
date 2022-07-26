#ifndef ACPI_H
#define ACPI_H

#include <boot/boot.h>
#include <hal/acpi/madt/madt.h>
#include <ktypes.h>

struct acpi_table_result
{
    struct apic_device_info apic;
};

typedef void* acpi_table_t;
void acpi_init(void);
acpi_table_t acpi_find_table(string_view identifier);

#endif // ACPI_H