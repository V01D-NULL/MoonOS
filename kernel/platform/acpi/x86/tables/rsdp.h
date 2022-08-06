#ifndef RSDP_H
#define RSDP_H

#include <mm/addr.h>
#include <platform/acpi/x86/acpi.h>
#include <base/base-types.h>
#include <moon.h>

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

void rsdp_init(phys_t rsdp_addr);
struct RSDP get_rsdp(void);
bool use_xsdt(void);

#endif // RSDP_H
