#include "madt.h"
#include <devices/serial/serial.h>
#include <hal/apic/apic.h>
#include <hal/pic/pic.h>
#include <mm/memdefs.h>
#include <printk.h>

#define LAPIC_ADDR_OVERRIDE 5 // Entry type 5 in the madt is the lapic address override

static const char* interrupt_device_id_map[6] = {
    "LAPIC",
    "I/O APIC",
    "I/O APIC Interrupt source override",
    "I/O APIC NMI source",
    "I/O APIC NMI",
    "LAPIC Address override"
};

static void enumarate_apic_devices(madt_t **madt);

void madt_init(acpi_table_t madt_base)
{
    madt_t *madt = (madt_t*)from_virt(GENERIC_CAST(uintptr_t, madt_base));

    // Strings aren't null terminated apparently, so we do it ourselves
    uint8_t oem_str[7];
    memcpy(GENERIC_CAST(uint8_t*, oem_str), GENERIC_CAST(uint8_t*, madt->sdt.oem_string), 6);
    oem_str[6] = '\0';

    uint8_t oem_table_id[9];
    memcpy(GENERIC_CAST(uint8_t*, oem_table_id), GENERIC_CAST(uint8_t*, madt->sdt.oem_table_id), 8);
    oem_table_id[8] = '\0';

    madt->local_apic_addr = (uint32_t)to_virt(GENERIC_CAST(uintptr_t, madt + 0x24));
    madt->local_apic_flags = (uint32_t)from_virt(GENERIC_CAST(uintptr_t, madt + 0x28));

    pic_disable();
    enumarate_apic_devices(&madt);

    printk("madt", "OEM String: '%s'\n", oem_str);
    printk("madt", "OEM Table ID: '%s'\n", oem_table_id);
    printk("madt", "LAPIC address: %lX\n", madt->local_apic_addr);
}

static void enumarate_apic_devices(madt_t** madt)
{
    uint8_t* madt_interrupt_devices = (uint8_t*)GENERIC_CAST(uintptr_t, *madt + 0x2c);
    uintptr_t iterations_required = GENERIC_CAST(uintptr_t, *madt) + (*madt)->sdt.length;

    do {
        switch (*madt_interrupt_devices) {
        case 0 ... 5: {
            printk("madt", "Detected %s\n", interrupt_device_id_map[*madt_interrupt_devices]);

            if (*madt_interrupt_devices == LAPIC_ADDR_OVERRIDE) {
                (*madt)->local_apic_addr64 = GENERIC_CAST(uint64_t, *madt_interrupt_devices + 0x4);
                // printk("madt", "LAPIC 64 bit address: %lX\n", );

            }

            madt_interrupt_devices += madt_interrupt_devices[1];
            break;
        }

        case 9:
            printk("madt", "Detected x2APIC\n");
            madt_interrupt_devices += madt_interrupt_devices[1];
            break;

        default:
            debug(true, "Invalid interrupt device detected, got %d, expected 0-5 or 9\n", *madt_interrupt_devices);
            madt_interrupt_devices += madt_interrupt_devices[1];
            break;
        }
    } while (GENERIC_CAST(uintptr_t, madt_interrupt_devices) < iterations_required);
}