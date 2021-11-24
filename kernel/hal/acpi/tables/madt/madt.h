#ifndef MADT_H
#define MADT_H

#include <hal/acpi/acpi.h>
#include <hal/acpi/tables/sdt.h>

typedef struct
{
    struct SDT sdt;
    uint32_t local_apic_addr;
    uint32_t local_apic_flags;
    uint64_t local_apic_addr64;
} gnu_pack_bytes madt_t;

void madt_init(acpi_table_t madt);

#endif // MADT_H