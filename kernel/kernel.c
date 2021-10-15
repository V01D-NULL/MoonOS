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
#include <boot/bootloader_stivale2.h>

#include <amd64/cpu.h>
#include <amd64/moon.h>
#include <amd64/bytes.h>

#include <int/gdt.h>
#include <int/idt.h>
#include <int/interrupts.h>

#include <drivers/io/serial.h>
#include <drivers/keyboard/keyboard.h>

#include <asm/x86/x86.h>

#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <libk/kassert.h>
#include <liballoc/bitmap.h>
#include <libgraphics/double-buffering.h>
#include <libgraphics/draw.h>

#include <util/common.h>
#include <util/ptr.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memdefs.h>
#include <mm/heap/heap.h>
#include <mm/linear/linear_alloc.h>

#include <trace/strace.h>
#include <hal/acpi/tables/rsdp.h>
#include <hal/acpi/acpi.h>

#include <sys/smp/smp.h>

#include "panic.h"
#include "printk.h"

void kmain(boot_info_t *bootvars)
{
    acpi_init(&bootvars->rsdp);
    smp_init(&bootvars->cpu);

    for (;;)
    {
        __asm__("hlt");
    }
}