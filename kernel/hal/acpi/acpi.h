#ifndef ACPI_H
#define ACPI_H

#include <boot/boot.h>
#include <hal/acpi/tables/madt/madt.h>

struct acpi_table_result
{
    struct apic_device_info apic;
};

typedef void* acpi_table_t;
struct acpi_table_result acpi_init(void);
acpi_table_t acpi_find_table(const char *identifier);

#endif // ACPI_H