#define PR_MODULE "rsdp"

#include "rsdp.h"
#include <moon-io/serial.h>
#include <boot/boot.h>
#include <panic.h>
#include <printk.h>
#include <mm/mm.h>
#include <base/base-types.h>
#include <stdalign.h>

static bool has_xsdt = false;
static struct RSDP rsdp;

void rsdp_verify_checksum(uint64_t rsdp_address);

void rsdp_init(void)
{
    auto rsdp_addr = BootContextGet().rsdp.rsdp_address;
    rsdp_verify_checksum(rsdp_addr);
    trace("RSDP Table: %llX\n", rsdp_addr + $high_vma);
    
    rsdp = *(struct RSDP*) rsdp_addr;

    // If this is omitted this member might print more than 6 bytes due to a missing terminator.
    // Is there really no specification to use a NULL terminating character in the entirety of ACPI?
    rsdp.oem_string[5] = '\0';
    trace("RSDP OEM: %s\n", rsdp.oem_string);

    /* XSDT */
    if (rsdp.revision >= 2)
    {
        trace("ACPI Version: 2.0+ (detection based on revision)\n");
        trace("XSDT address: %llX\n", rsdp.xsdt_address);
        has_xsdt = true;
    }
    /* RSDT */
    else
    {
        trace("Revision: %d (Assuming ACPI version 1.0)\n", rsdp.revision);
        trace("RSDT address: %llX\n", rsdp.rsdt_address);
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