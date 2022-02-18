#include "rsdp.h"
#include <devices/serial/serial.h>
#include <util/ptr.h>
#include <stdbool.h>
#include <panic.h>
#include <printk.h>
#include <mm/mm.h>

static bool has_xsdt = false;
static struct RSDP rsdp;

void rsdp_verify_checksum(uint64_t rsdp_address);

void rsdp_init(boot_rsdp_t *boot_rsdp_table)
{
    rsdp_verify_checksum(boot_rsdp_table->rsdp_address);
    printk("acpi-rsdp", "RSDP Table: %llX\n", to_higher_half(boot_rsdp_table->rsdp_address, DATA));
    
    rsdp = *(struct RSDP*) boot_rsdp_table->rsdp_address;

    // If this is omitted this member might print more than 6 bytes due to a missing terminator.
    // Is there really no specification to use a NULL terminating character in the entirety of ACPI?
    rsdp.oem_string[5] = '\0';
    printk("acpi-rsdp", "RSDP OEM: %s\n", rsdp.oem_string);

    /* XSDT */
    if (rsdp.revision >= 2)
    {
        printk("acpi-rsdp", "ACPI Version: 2.0+ (detection based on revision)\n");
        printk("acpi-rsdp", "XSDT address: %llX\n", to_higher_half(rsdp.xsdt_address, DATA));
        has_xsdt = true;
    }
    /* RSDT */
    else
    {
        printk("acpi-rsdp", "Revision: %d (Assuming ACPI version 1.0)\n", rsdp.revision);
        printk("acpi-rsdp", "RSDT address: %llX\n", to_higher_half(rsdp.rsdt_address, DATA));
    }
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
        panic("RSDP checksum is invalid (%X)", checksum & 0xFF);
    }
}

bool use_xsdt(void)
{
    return has_xsdt;
}

struct RSDP get_rsdp(void)
{
    return rsdp;
}