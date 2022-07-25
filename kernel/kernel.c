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

#include <hal/time/sleep.h>
#include <hal/acpi/acpi.h>

#include <proc/daemon/load.h>
#include <proc/uspace/userspace.h>
#include <proc/uspace/syscalls.h>
#include <proc/smp/smp.h>

#include "panic.h"
#include "printk.h"

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
    kmalloc_init();
    acpi_init();
    sleep_init();

    init_percpu(bootvars->rbp);
    init_syscalls();

    pr_info("Detected %d modules\n", mods->module_count);
    pr_info("Module string: %s\n", mods->modules[0].string);

    load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);

    for (;;)
    {
        asm("cli;hlt");
    }
}
