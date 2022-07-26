#define PR_MODULE "main"

#include <stivale2.h>
#include <boot/boot.h>

#include <amd64/cpu.h>
#include <mm/dynamic/kmalloc.h>

#include <hal/time/sleep.h>
#include <hal/acpi/acpi.h>

#include <proc/sched/scheduler.h>
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

    trace("Detected %d modules\n", mods->module_count);
    trace("Module string: %s\n", mods->modules[0].string);

    load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);
	sched_init();

    for (;;)
    {
        asm("hlt");
    }
}
