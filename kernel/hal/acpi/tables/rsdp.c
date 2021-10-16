#include "rsdp.h"
#include <drivers/io/serial.h>
#include <util/ptr.h>
#include <stdbool.h>
#include <panic.h>
#include <printk.h>

#define READ_PTR(in_ptr)  tmp1 = *in_ptr++, tmp2 = *in_ptr++, tmp3 = *in_ptr++, tmp4 = *in_ptr++
#define CONCAT_INT32(out)   out = tmp1 << 24 | tmp2 << 16 | tmp3 << 8 | tmp4

struct RSDP rsdp;

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

/* Parse RSDT table, if available the xsdt table is parsed instead */
void rsdp_init(boot_rsdp_t *boot_rsdp_table)
{
    rsdp_verify_checksum(boot_rsdp_table->rsdp_address);

    printk("acpi-rsdp", "RSDP Table: %llX\n", boot_rsdp_table->rsdp_address);
    uint8_t *addr = (uint8_t *)boot_rsdp_table->rsdp_address;

    // I could comment all this code, but I could also just link the ACPI specification :^)
    // https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/05_ACPI_Software_Programming_Model/ACPI_Software_Programming_Model.html#rsdp-structure
    for (int i = 0; i < 8; i++)
        debug(false, "%c",*addr++);
    debug(false,"\n");
        // rsdp.signature[i] = *addr++;

    rsdp.checksum = *addr++;
    
    int counter = 0;
    for (int n = 0; n < 6; n++)
        rsdp.oem_string[counter++] = *addr++;

    rsdp.revision = *addr++;
    // (void)*addr++;
    int32_t tmp1, tmp2, tmp3, tmp4;
    // for (int i = 0; i < 4; i++)
    // {
    //     debug(false, "%X", *addr++);
    // }
    // debug(false, "\n");
    // tmp1 = *addr++, tmp2 = *addr++, tmp3 = *addr++, tmp4 = *addr++;
    rsdp.rsdt_address = 0xE22FEF;//tmp1 << 24 | tmp2 << 16 | tmp3 << 8 | tmp4;
    // READ_PTR(addr);
    // CONCAT_INT32(rsdp.rsdt_address);

    printk("acpi-rsdp", "Signature: %s\n", rsdp.signature);
    printk("acpi-rsdp", "Oem string: %s\n", rsdp.oem_string);

    /* XSDT */
    if (is_acpi_v2(boot_rsdp_table->rsdp_address))
    {
        READ_PTR(addr);
        CONCAT_INT32(rsdp.length);

        READ_PTR(addr); //Read tmp1-4, i.e. the first 4 bytes of the 8 byte wide xsdt address
        int64_t tmp5 = *addr++, tmp6 = *addr++, tmp7 = *addr++, tmp8 = *addr++;
        rsdp.xsdt_address = (tmp8 << 56 | tmp7 << 48 | tmp6 << 40 | tmp5 << 32 | tmp4 << 24 | tmp3 << 16 | tmp2 << 8 | tmp1);

        printk("acpi-xsdt", "ACPI Version: 2.0+ (detection based on revision)\n");
        printk("acpi-rsdp", "RSDP table size: %d\n", rsdp.length);
        printk("acpi-rsdp", "XSDT address: %llX\n", rsdp.xsdt_address);
    }
    /* RSDT */
    else
    {
        printk("acpi-rsdp", "Revision: %d (Assuming ACPI version 1.0)\n", rsdp.revision);
        printk("acpi-rsdp", "RSDT address: %llX\n", rsdp.rsdt_address);
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
        panic("RSDP checksum is invalid (%X)\n", checksum & 0xFF);
    }
}

struct RSDP get_rsdp()
{
    return rsdp;
}