#ifndef ACPI_H
#define ACPI_H

#include <mm/addr.h>
#include <platform/acpi/x86/madt/madt.h>
#include <base/base-types.h>

struct acpi_table_result
{
    struct apic_device_info apic;
};

typedef void* AcpiTable;
void acpi_init(phys_t rsdp);
AcpiTable acpi_find_table(string_view identifier);

#endif // ACPI_H