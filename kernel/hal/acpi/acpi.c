#include "acpi.h"
#include <hal/acpi/tables/rsdp.h>

void acpi_init(boot_rsdp_t *boot_rsdp_table)
{
    rsdp_init(boot_rsdp_table);
}