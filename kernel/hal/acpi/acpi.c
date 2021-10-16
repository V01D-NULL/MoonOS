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
}

int _strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        char c1 = s1[i], c2 = s2[i];
        if (c1 != c2)
            return c1 - c2;
        if (!c1)
            return 0;
    }

    return 0;
}


acpi_table_t acpi_find_table(const char *identifier)
{

    struct RSDP rsdp = get_rsdp();
    struct RSDT *rsdt = (struct RSDT*) (uintptr_t) rsdp.rsdt_address;
    debug(1, "%lX\n", rsdp.rsdt_address);
    size_t entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

    debug(1, "entries: %d\n", entries);
    for (size_t i = 0; i < entries; i++)
    {
        struct SDT *sdt = (struct SDT*) (uintptr_t)rsdt->next[i];
        
        if (!_strncmp(sdt->signature, (char*)identifier, 4))
        {
            return (acpi_table_t) sdt;
        }
    }

    debug(true, "Could not find table '%s'\n", identifier);
    return NULL;
}