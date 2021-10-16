#ifndef RSDP_H
#define RSDP_H

#include <stdint.h>
#include <amd64/moon.h>
#include <boot/bootloader_stivale2.h>
#include "sdt.h"
#include <hal/acpi/acpi.h>

#define ACPI_V1_LEGACY 0 /* Revision 0 == ACPI version 1.0 or lower */

struct RSDP
{
    // Version 1.0
    char signature[8];
    uint8_t checksum;
    char oem_string[8];
    uint8_t revision;
    uint32_t rsdt_address;

    // Version 2.0+
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __pack_bytes;

void rsdp_init(boot_rsdp_t *boot_rsdp_table);
struct RSDP get_rsdp();

#endif // RSDP_H
