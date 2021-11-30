#ifndef MADT_H
#define MADT_H

#include <hal/acpi/tables/sdt.h>
#include <amd64/moon.h>

typedef struct
{
    struct SDT sdt;
    uint32_t local_apic_addr;
    uint32_t local_apic_flags;
} gnu_pack_bytes madt_t;

struct ioapic_dev
{
    /* Can be ignored */
    uint8_t entry_type;
    uint8_t record_len;

    /* I/O APIC specific data */
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t address;
    uint32_t gsib; 
} gnu_pack_bytes;

struct apic_device_info
{
    uint64_t lapic_addr;
    uint32_t usable_ioapics;
    struct ioapic_dev **ioapics;
};

struct apic_device_info madt_init(void *madt);

#endif // MADT_H