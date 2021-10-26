#include "madt.h"
#include <printk.h>

void madt_init(acpi_table_t madt)
{
    struct SDT ptr = *(struct SDT*)madt;
    
    char oem_str[7];
    for (int i = 0; i < 6; i++)
        oem_str[i] = ptr.oem_string;
    oem_str[5] = '\0';

    printk("madt", "OEM String: %s\n", oem_str);
}