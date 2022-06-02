/**
 * @mainpage
 * @file kernel.c
 * @author Tim (V01D)
 * @brief This is where all the magic happens :)
 * @version 0.1
 * @date 2021-04-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#define PR_MODULE "main"

#include <stdint.h>
#include <stddef.h>

#include <stivale2.h>
#include <boot/boot.h>

#include <amd64/cpu.h>
#include <mm/dynamic/kmalloc.h>

#include <hal/acpi/tables/rsdp.h>
#include <hal/acpi/acpi.h>
#include <hal/apic/apic.h>

#include <proc/daemon/load.h>
#include <proc/uspace/userspace.h>
#include <proc/uspace/syscalls.h>
#include <proc/smp/smp.h>

#include <devices/serial/serial.h>

#include "panic.h"
#include "printk.h"

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
    init_percpu(bootvars->rbp); // Every logical core (which each calls init_percpu) shares it's stack with the syscall handler
    init_syscalls();

    pr_info("Detected %d modules\n", mods->module_count);
    pr_info("Module string: %s\n", mods->modules[0].string);

    pr_info("LAPIC Version register: %x\n", lapic_read(0x30));
    auto reg = lapic_read(0x30);
    pr_info("version: 0x%x, max LVT entry: %x\n", reg & 0xFF, reg >> 16);

    auto version = reg & 0xff;
    pr_info("LAPIC is %s APIC\n", (version >= 0x10 && version <= 0x15) ? "an integrated" : "a 82489DX discrete");
    // smp_init(&bootvars->cpu);
    load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);

    // Note: For a zoned buddy allocator every zone must be 2 MiB (max allocation for one buddy tree)

    for (;;)
    {
        asm("cli;hlt");
    }
}
