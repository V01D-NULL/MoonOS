#ifndef RSDP_H
#define RSDP_H

#include <stdint.h>
#include <amd64/moon.h>
#include <boot/boot.h>
#include <hal/acpi/acpi.h>
#include <stdbool.h>

struct RSDP
{
    // Version 1.0
    char signature[8];
    uint8_t checksum;
    char oem_string[6];
    uint8_t revision;
    uint32_t rsdt_address;

    // Version 2.0+
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} PACKED;

void rsdp_init(void);
struct RSDP get_rsdp(void);
bool use_xsdt(void);

#endif // RSDP_H
