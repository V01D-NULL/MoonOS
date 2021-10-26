#ifndef MADT_H
#define MADT_H

#include <hal/acpi/acpi.h>
#include <hal/acpi/tables/sdt.h>

typedef struct
{
    struct SDT sdt;
} madt_t;


void madt_init(acpi_table_t madt);

#endif // MADT_H