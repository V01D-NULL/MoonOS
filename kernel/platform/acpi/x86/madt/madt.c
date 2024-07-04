#define PR_MODULE "madt"

#include "madt.h"
#include <arch/x86/hardware/legacy/pic/pic.h>
#include <base/string.h>
#include <mm/phys.h>
#include <moon-io/serial.h>
#include <platform/acpi/x86/acpi.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>

/* Entry types */
#define LAPIC_ADDR_OVERRIDE \
    5  // Entry type 5 in the madt is the lapic address override
#define IOAPIC 1

static string_view interrupt_device_id_map[6] = {
    "LAPIC",
    "I/O APIC",
    "I/O APIC Interrupt source override",
    "I/O APIC NMI source",
    "I/O APIC NMI",
    "LAPIC Address override"};

static struct apic_device_info apic_dev;
static void                    enumarate_apic_devices(madt_t **madt);

struct apic_device_info madt_init(void *madt_base)
{
    madt_t *madt     = (madt_t *)((uintptr_t)madt_base);
    apic_dev.ioapics = (struct ioapic_dev **)
        arch_alloc_page();  // TODO: Make this a heap allocation!

    // Strings aren't null terminated apparently, so we do it ourselves
    uint8_t oem_str[7];
    memcpy((uint8_t *)oem_str, (uint8_t *)madt->sdt.oem_string, 6);
    oem_str[6] = '\0';

    uint8_t oem_table_id[9];
    memcpy((uint8_t *)oem_table_id, (uint8_t *)madt->sdt.oem_table_id, 8);
    oem_table_id[8] = '\0';

    apic_dev.lapic_addr = madt->local_apic_addr;

    pic_disable();
    enumarate_apic_devices(&madt);

    trace(TRACE_MISC, "OEM String: '%s'\n", oem_str);
    trace(TRACE_MISC, "OEM Table ID: '%s'\n", oem_table_id);

    return apic_dev;
}

static void enumarate_apic_devices(madt_t **madt)
{
    uint8_t  *madt_interrupt_devices = (uint8_t *)((uintptr_t)*madt + 0x2c);
    uintptr_t iterations_required    = ((uintptr_t)*madt) + (*madt)->sdt.length;

    do
    {
        switch (*madt_interrupt_devices)
        {
            case 0 ... 5:
            {
                trace(TRACE_MISC,
                      "Detected %s\n",
                      interrupt_device_id_map[*madt_interrupt_devices]);

                if (*madt_interrupt_devices == LAPIC_ADDR_OVERRIDE)
                {
                    debug(true, "apic_dev.lapic_addr: %p", apic_dev.lapic_addr);
                    apic_dev.lapic_addr = (uint64_t)madt_interrupt_devices + 4;
                }

                if (*madt_interrupt_devices == IOAPIC)
                {
                    apic_dev.ioapics[apic_dev.usable_ioapics++] =
                        ((struct ioapic_dev *)madt_interrupt_devices);
                }

                madt_interrupt_devices += madt_interrupt_devices[1];
                break;
            }

            case 9:
                printk("madt", "Detected x2APIC\n");
                madt_interrupt_devices += madt_interrupt_devices[1];
                break;

            default:
                trace(TRACE_MISC,
                      "Invalid interrupt device detected, got %d, expected 0-5 "
                      "or 9\n",
                      *madt_interrupt_devices);
                madt_interrupt_devices += madt_interrupt_devices[1];
                break;
        }
    } while (((uintptr_t)madt_interrupt_devices) < iterations_required);
}
