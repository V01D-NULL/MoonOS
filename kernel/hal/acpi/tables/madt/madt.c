#include "madt.h"
#include <printk.h>
#include <devices/serial/serial.h>
#include <mm/memdefs.h>

void madt_init(acpi_table_t madt)
{
    struct SDT *ptr = (struct SDT*)from_virt(GENERIC_CAST(uintptr_t, madt));
    
    // Strings aren't null terminated apparently, so we do it ourselves
    uint8_t oem_str[7];
    memcpy(GENERIC_CAST(uint8_t *, oem_str), GENERIC_CAST(uint8_t *, ptr->oem_string), 6);
    oem_str[6] = '\0';

    uint8_t oem_table_id[9];
    memcpy(GENERIC_CAST(uint8_t *, oem_table_id), GENERIC_CAST(uint8_t *, ptr->oem_table_id), 8);
    oem_table_id[8] = '\0';


    printk("madt", "OEM String: %s\n", oem_str);
    printk("madt", "OEM Table ID: %s\n", oem_table_id);
}