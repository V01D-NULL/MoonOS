#ifndef ACPI_H
#define ACPI_H

#include <boot/boot.h>
#include <platform/acpi/x86/madt/madt.h>
#include <base/base-types.h>

struct acpi_table_result
{
    struct apic_device_info apic;
};

typedef void* acpi_table_t;
void acpi_init(void);
acpi_table_t acpi_find_table(string_view identifier);

#endif // ACPI_H