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
#include <amd64/validity.h>
#include <amd64/bytes.h>

#include <int/gdt.h>
#include <int/idt.h>
#include <int/interrupts.h>

#include <drivers/io/serial.h>
#include <drivers/vbe/vbe.h>

#include <asm/x86/x86.h>

#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <libk/kassert.h>
#include <liballoc/bitmap.h>
#include <libgraphics/double-buffering.h>

#include <util/common.h>
#include <util/ptr.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memdefs.h>
#include <mm/linear/linear_alloc.h>

#include <trace/strace.h>
#include "panic.h"
#include "printk.h"
#include <hal/acpi/tables/rsdp.h>

extern gfx_header_t gfx_h;
void kmain(boot_info_t *bootvars)
{
    /* Init the CPU hardware struct */
    cpu_info_init(*bootvars);
    cpu_info();

    vmm_init(check_la57());
    create_safe_panic_area();

    rsdp_init(&bootvars->rsdp);
    
    double_buffering_init();
    kernel_log_init();

    range_t r = pmm_alloc_range(2);
    debug(true, "base: %llX, top: %llX\n", r.base, r.limit);

    for (;;)
    {
        __asm__("hlt");
    }
}
