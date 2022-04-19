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
#include <amd64/moon.h>
#include <amd64/bytes.h>
#include <amd64/msr.h>

#include <int/gdt.h>
#include <int/idt.h>
#include <int/interrupts.h>

#include <devices/serial/serial.h>
#include <drivers/keyboard/keyboard.h>

#include <asm/x86/x86.h>

#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <libk/kassert.h>
#include <liballoc/bitmap.h>
#include <libgraphics/double-buffering.h>
#include <libgraphics/draw.h>

#include <util/common.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/slab.h>
#include <mm/mm.h>
#include <mm/buddy/buddy.h>

#include <trace/strace.h>
#include <hal/acpi/tables/rsdp.h>
#include <hal/acpi/acpi.h>
#include <hal/apic/apic.h>

#include <proc/daemon/load.h>
#include <proc/uspace/userspace.h>
#include <proc/uspace/syscalls.h>
#include <proc/elf/elf.h>

#include "panic.h"
#include "printk.h"

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
    init_percpu(bootvars->rbp); // Every logical core (which each calls init_percpu) shares it's stack with the syscall handler
    init_syscalls();

	printk("main", "Detected %d modules\n", mods->module_count);
	printk("main", "Module string: %s\n", mods->modules[0].string);
    
    printk("main", "0x%lx\n", BootContextGet().rbp);

	lapic_init(acpi_init().apic);
    load_daemon((const uint8_t*)mods->modules[0].begin, mods->modules[0].string);
	
    // smp_init(&bootvars->cpu);

    for (;;)
	{
        asm("cli;hlt");
	}
}
