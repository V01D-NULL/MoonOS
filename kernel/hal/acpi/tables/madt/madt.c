#include "madt.h"
#include <hal/acpi/acpi.h>
#include <devices/serial/serial.h>
#include <hal/apic/apic.h>
#include <hal/pic/pic.h>
#include <mm/memdefs.h>
#include <printk.h>

/* Entry types */
#define LAPIC_ADDR_OVERRIDE 5 // Entry type 5 in the madt is the lapic address override
#define IOAPIC              1

static const char* interrupt_device_id_map[6] = {
    "LAPIC",
    "I/O APIC",
    "I/O APIC Interrupt source override",
    "I/O APIC NMI source",
    "I/O APIC NMI",
    "LAPIC Address override"
};

static struct apic_device_info apic_dev;
static void enumarate_apic_devices(madt_t **madt);

struct apic_device_info madt_init(void *madt_base)
{
    madt_t *madt = (madt_t*)from_higher_half(GENERIC_CAST(uintptr_t, madt_base), DATA);
    apic_dev.ioapics = pmm_alloc(); // TODO: Make this a heap allocation!

    // Strings aren't null terminated apparently, so we do it ourselves
    uint8_t oem_str[7];
    memcpy(GENERIC_CAST(uint8_t*, oem_str), GENERIC_CAST(uint8_t*, madt->sdt.oem_string), 6);
    oem_str[6] = '\0';

    uint8_t oem_table_id[9];
    memcpy(GENERIC_CAST(uint8_t*, oem_table_id), GENERIC_CAST(uint8_t*, madt->sdt.oem_table_id), 8);
    oem_table_id[8] = '\0';

    apic_dev.lapic_addr = madt->local_apic_addr;

    pic_disable();
    enumarate_apic_devices(&madt);

    printk("madt", "OEM String: '%s'\n", oem_str);
    printk("madt", "OEM Table ID: '%s'\n", oem_table_id);

    return apic_dev;
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
                apic_dev.lapic_addr = (uint64_t)madt_interrupt_devices+4;
            }

            if (*madt_interrupt_devices == IOAPIC) {
                apic_dev.ioapics[apic_dev.usable_ioapics++] = GENERIC_CAST(struct ioapic_dev*, madt_interrupt_devices);
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