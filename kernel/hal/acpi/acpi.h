#ifndef ACPI_H
#define ACPI_H

#include <boot/bootloader_stivale2.h>
#include <hal/acpi/tables/madt/madt.h>

struct acpi_table_result
{
    struct apic_device_info apic;
};

typedef void* acpi_table_t;
struct acpi_table_result acpi_init(boot_rsdp_t *boot_rsdp_table);
acpi_table_t acpi_find_table(const char *identifier);

#endif // ACPI_H