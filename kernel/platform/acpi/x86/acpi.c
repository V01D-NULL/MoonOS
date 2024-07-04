#include "acpi.h"
#include <platform/acpi/x86/madt/madt.h>
#include <platform/acpi/x86/tables/rsdp.h>
#include <platform/acpi/x86/tables/rsdt.h>

#include <base/base-types.h>
#include <base/string.h>
#include <moon-io/serial.h>
#include <panic.h>
#include <printk.h>

bool acpi_validate_sdt_checksum(struct SDT *sdt);

void acpi_init(phys_t rsdp)
{
    rsdp_init(rsdp);
}

AcpiTable acpi_find_table(string_view identifier)
{
    struct RSDP rsdp = get_rsdp();

    struct RSDT *rsdt = (struct RSDT *)va(rsdp.xsdt_address);
    struct XSDT *xsdt = NULL;
    struct SDT   header;

    if (use_xsdt())
    {
        xsdt   = (struct XSDT *)va(rsdp.xsdt_address);
        header = xsdt->header;
    }
    else
    {
        rsdt   = (struct RSDT *)va(rsdp.rsdt_address);
        header = rsdt->header;
    }

    size_t entries = (header.length - sizeof(header)) / (use_xsdt() ? 8 : 4);

    for (size_t i = 0; i < entries; i++)
    {
        struct SDT *sdt =
            (struct SDT *)va(use_xsdt() ? xsdt->next[i] : rsdt->next[i]);

        if (!strncmp(sdt->signature, (string)identifier, 4) &&
            acpi_validate_sdt_checksum(sdt))
        {
            return (AcpiTable)sdt;
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
        sum += ((uint8_t *)sdt)[i];
    }
    return sum == 0;
}