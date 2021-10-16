#include "acpi.h"
#include <hal/acpi/tables/rsdp.h>
#include <hal/acpi/tables/rsdt.h>

#include <mm/memdefs.h>
#include <drivers/io/serial.h>
#include <libk/kstring.h>
#include <stdbool.h>

void acpi_init(boot_rsdp_t *boot_rsdp_table)
{
    rsdp_init(boot_rsdp_table);
    
    acpi_table_t result;
    if ((result = acpi_find_table("FACP")) != NULL)
    {
        debug(true, "Found FADT table @ %p\n", result);
    }
}

acpi_table_t acpi_find_table(const char *identifier)
{

    struct RSDP rsdp = get_rsdp();
    struct RSDT *rsdt = (struct RSDT*) (uintptr_t) rsdp.rsdt_address;

    size_t entries = (rsdt->header.length - sizeof(rsdt->header)) / (use_xsdt() ? 8 : 4);
    
    for (size_t i = 0; i < entries; i++)
    {
        struct SDT *sdt = (struct SDT*) (uintptr_t)rsdt->next[i];
        
        if (!strncmp(sdt->signature, (char*)identifier, 4))
        {
            return (acpi_table_t) to_virt((uintptr_t)sdt);
        }
    }

    debug(true, "Could not find table '%s'\n", identifier);
    return NULL;
}