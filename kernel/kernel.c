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

#include <devices/serial/serial.h>

#include "panic.h"
#include "printk.h"

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
    init_percpu(bootvars->rbp); // Every logical core (which each calls init_percpu) shares it's stack with the syscall handler
    init_syscalls();

    pr_info("Detected %d modules\n", mods->module_count);
    pr_info("Module string: %s\n", mods->modules[0].string);

    printk("\n" PR_MODULE,"kmem_cache_alloc test:\n");
    auto cache = kmem_cache_create("foo", 512, 0);

    if (cache != NULL)
    {
        auto ptr = kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA | KMEM_NO_GROW);
        
        pr_info("Allocated memory via kmem_cache_alloc: 0x%p\n", ptr);
        kmem_cache_free(cache, ptr);
        kmem_cache_dump(cache, -1);

        kmem_cache_destroy(cache);
        // kmem_cache_alloc(cache, KMEM_PANIC); // Attempted to allocate after destroying the cache and set the panic flag - this should fail and panic.
    }

    lapic_init(acpi_init().apic);
    load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);

    // smp_init(&bootvars->cpu);

    for (;;)
    {
        asm("cli;hlt");
    }
}
