#include "acpi.h"
#include <hal/acpi/tables/rsdp.h>
#include <hal/acpi/tables/rsdt.h>
#include <hal/acpi/tables/madt/madt.h>

#include <mm/mm.h>
#include <devices/serial/serial.h>
#include <libk/kstring.h>
#include <printk.h>
#include <stdbool.h>
#include <panic.h>

bool acpi_validate_sdt_checksum(struct SDT *sdt);

struct acpi_table_result acpi_init(boot_rsdp_t *boot_rsdp_table)
{
    struct acpi_table_result result;

    rsdp_init(boot_rsdp_table);
    
    acpi_table_t madt;
    if ((madt = acpi_find_table("APIC")) != NULL)
    {
        printk("acpi", "Found MADT table @ %p\n", madt);
        result.apic = madt_init(madt);
    }
    else
    {
        panic("No MADT table, no APIC - Refusing to boot");
    }

    return result;
}

acpi_table_t acpi_find_table(const char *identifier)
{
    struct RSDP rsdp = get_rsdp();
    struct RSDT *rsdt = (struct RSDT*) (uintptr_t) rsdp.rsdt_address;
    size_t entries = (rsdt->header.length - sizeof(rsdt->header)) / (use_xsdt() ? 8 : 4);
    
    for (size_t i = 0; i < entries; i++)
    {
        struct SDT *sdt = (struct SDT*) (uintptr_t)rsdt->next[i];
        
        if (!strncmp(sdt->signature, (char*)identifier, 4) && acpi_validate_sdt_checksum(sdt))
        {
            return (acpi_table_t) ((uintptr_t)sdt + $high_vma);
        }
    }

    printk("acpi", "Could not find table '%s'\n", identifier);
    return NULL;
}

bool acpi_validate_sdt_checksum(struct SDT *sdt)
{
    uint8_t sum = 0;
    
    for (size_t i = 0; i < sdt->length; i++)
    {
        sum += ((uint8_t*) sdt)[i];
    }
    return sum == 0;
}