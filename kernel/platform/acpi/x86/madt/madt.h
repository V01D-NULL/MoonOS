#ifndef MADT_H
#define MADT_H

#include <moon-ds/containers.h>
#include <moon.h>
#include <platform/acpi/x86/tables/sdt.h>

packed_struct$(madt_t, {
    struct SDT sdt;
    uint32_t   local_apic_addr;
    uint32_t   local_apic_flags;
});

struct ioapic_dev
{
    /* Can be ignored */
    uint8_t entry_type;
    uint8_t record_len;

    /* I/O APIC specific data */
    uint8_t  io_apic_id;
    uint8_t  reserved;
    uint32_t address;
    uint32_t gsib;
} PACKED;

struct apic_device_info
{
    uint64_t lapic_addr;
    uint32_t usable_ioapics;
    vec(struct ioapic_dev) ioapics;
};

struct apic_device_info madt_init(void *madt);

#endif  // MADT_H