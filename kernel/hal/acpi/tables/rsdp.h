#ifndef RSDP_H
#define RSDP_H

#include <stdint.h>
#include <amd64/moon.h>
#include <boot/bootloader_stivale2.h>

#define ACPI_V1_LEGACY 0 /* Revision 0 == ACPI version 1.0 or lower */

struct rsdp_tableV1
{
    char signature[8];
    uint8_t checksum;
    char oem_string[8];
    uint8_t revision;
    uint32_t rsdt_address;
} __pack_bytes;

struct rsdp_tableV2
{
    struct rsdp_tableV1 rsdp;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __pack_bytes;

struct xstd_table
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_string[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creatorID;
    uint32_t creato_revision;
} __pack_bytes;

void rsdp_init(boot_rsdp_t *boot_rsdp_table);

#endif // RSDP_H
