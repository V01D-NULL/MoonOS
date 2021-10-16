#ifndef ACPI_H
#define ACPI_H

#include <boot/bootloader_stivale2.h>

typedef void* acpi_table_t;
void acpi_init(boot_rsdp_t *boot_rsdp_table);
acpi_table_t acpi_find_table(const char *identifier);

#endif // ACPI_H