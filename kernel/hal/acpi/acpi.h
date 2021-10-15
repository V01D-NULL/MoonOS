#ifndef ACPI_H
#define ACPI_H

#include <boot/bootloader_stivale2.h>

void acpi_init(boot_rsdp_t *boot_rsdp_table);

#endif // ACPI_H