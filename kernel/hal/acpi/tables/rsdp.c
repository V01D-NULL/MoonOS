#include "rsdp.h"
#include <drivers/vbe/vbe.h>
#include <drivers/io/serial.h>
#include <util/ptr.h>
#include <stdbool.h>
#include <panic.h>

struct rsdp_tableV1 rsdp_v1;
struct rsdp_tableV2 rsdp_v2;
struct xstd_table xstd;

void xsdt_init(uint64_t rsdp_base);
void rsdp_verify_checksum(uint64_t rsdp_address);

static int is_acpi_v2(uint64_t rsdp_base)
{
    uint8_t *rsdp = GENERIC_CAST(uint8_t *, rsdp_base);
    int revision = 0;
    for (int i = 0; i < 16; i++)
        revision = *rsdp++;

    debug(true, "ACPI Revision: %d\n", revision);
    return revision;
}

/* Parse RSDT table, if availbable the xstd table is parsed instead */
void rsdp_init(boot_rsdp_t *boot_rsdp_table)
{
    printk("acpi-rsdp", "RSDP Table: %llX\n", boot_rsdp_table->rsdp_address);
    uint8_t *addr = (uint8_t *)boot_rsdp_table->rsdp_address;

    /* RSDT */
    int i = 0;
    for (; i < 7; i++)
        rsdp_v1.signature[i] = *addr++;

    rsdp_v1.checksum = *addr++;
    i++;

    int counter = 0;
    for (int n = 0; n < 7; n++)
        rsdp_v1.oem_string[counter++] = *addr++;

    rsdp_v1.revision = *addr++;
    rsdp_v1.rsdt_address = (size_t)boot_rsdp_table->rsdp_address + (*addr + 4);

    printk("acpi-rsdp", "Signature: %s\n", rsdp_v1.signature);
    printk("acpi-rsdp", "Checksum: %d | %X\n", rsdp_v1.checksum, rsdp_v1.checksum);
    printk("acpi-rsdp", "Oem string: %s\n", rsdp_v1.oem_string);

    /* XSTD */
    if (is_acpi_v2(boot_rsdp_table->rsdp_address) > ACPI_V1_LEGACY)
    {
        printk("acpi-xstd", "ACPI Version: 2.0+ (detection based on revision)\n");
        printk("acpi-rsdp", "RSDT address: %llX\n", rsdp_v1.rsdt_address);

        xsdt_init(boot_rsdp_table->rsdp_address);
        return;
    }

    printk("acpi-rsdp", "Revision: %d (Assuming ACPI version 1.0)\n", rsdp_v1.revision);
    printk("acpi-rsdp", "RSDT address: %llX\n", rsdp_v1.rsdt_address);
    rsdp_verify_checksum(boot_rsdp_table->rsdp_address);
}

void rsdp_verify_checksum(uint64_t rsdp_address)
{
    int checksum = 0;
    uint8_t *ptr = (uint8_t *)rsdp_address;

    debug(true, "RSDP checksum raw:\n");
    for (int i = 0; i < 20; i++)
    {
        debug(false, " %x", *ptr);
        checksum += *ptr++;
    }
    debug(false, "\n");

    debug(true, "Calculated checksum: 0x%lX\n", checksum);
    if ((checksum & 0xFF) == 0x00)
    {
        debug(true, "Checksum valid\n");
    }
    else
    {
        panic("RSDP checksum is invalid (%lX)\n", checksum & 0xFF);
    }
}

void xsdt_init(uint64_t rsdp_base)
{
    uint8_t *addr = (uint8_t *)rsdp_base + 20;
    rsdp_v2.rsdp = rsdp_v1;
    rsdp_v2.length = *addr;
    rsdp_v2.xsdt_address = rsdp_base;

    printk("acpi-xstd", "Length: %d\n", rsdp_v2.length);
    rsdp_verify_checksum(rsdp_base);
}
