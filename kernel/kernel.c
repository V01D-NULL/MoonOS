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

    pr_info("\nkmem_cache_alloc test:\n");
    auto cache = kmem_cache_create("foo", 512, 0);

    if (cache != NULL)
    {
        // Allocate all memory from one slab
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        pr_info("kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
        
        // All memory in one slab has been allocated, but the slab will grow automatically.
        pr_info("(New slab creation here) kmem_cache_alloc: 0x%p\n", kmem_cache_alloc(cache, KMEM_PANIC | KMEM_HIGH_VMA));
    }

    pr_info("Ref count: %d\n", cache->nodes->refcount);

    // lapic_init(acpi_init().apic);
    // load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);

    // smp_init(&bootvars->cpu);

    for (;;)
    {
        asm("cli;hlt");
    }
}
