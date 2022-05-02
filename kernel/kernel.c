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

#include <devices/serial/serial.h>

#include "panic.h"
#include "printk.h"

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
    init_percpu(bootvars->rbp); // Every logical core (which each calls init_percpu) shares it's stack with the syscall handler
    init_syscalls();

	printk("main", "Detected %d modules\n", mods->module_count);
	printk("main", "Module string: %s\n", mods->modules[0].string);
    
    auto cache = kmem_cache_create("foo", 512, 0);
    
    debug(true, "Dumping bufctl freelists...\n");
    list_foreach(slab, next, cache->nodes)
    {
        debug(true, "Found slab at %p\n", slab);
        list_foreach(out, next, slab->freelist)
        {
            debug(true, "out->pa_ptr: %lx\n", out->pa_ptr);
        }
    }

	// lapic_init(acpi_init().apic);
    // load_daemon((const uint8_t*)mods->modules[0].begin, mods->modules[0].string);
	
    // smp_init(&bootvars->cpu);

    for (;;)
	{
        asm("cli;hlt");
	}
}
