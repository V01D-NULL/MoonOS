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
#include <drivers/gfx/gfx.h>

#include <hal/apic.h>

#include <asm/x86/x86.h>

#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <libk/kassert.h>
#include <liballoc/bitmap.h>

#include <util/common.h>
#include <util/ptr.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memdefs.h>
#include <mm/buff/linear_alloc.h>

#include "panic.h"

void banner();

void kmain(boot_info_t *bootvars)
{
    init_gdt();
    init_idt();

    /* Init the VESA printing routines, font loading, etc */
    // gfx_init(*bootvars, 0x00, 0xf0bacde);
    gfx_init(*bootvars, 0xffffff, 0x00);

    /* Init the CPU hardware struct */
    cpu_info_init(*bootvars);
    banner();
    cpu_info();

    pmm_init(bootvars->mmap.memmap, bootvars->mmap.entries);
    vmm_init(check_la57());

    //vmm_remap test:
    uint64_t *ptr = (uint64_t *)(0xA000000000);
    *ptr = 1; // force page fault by writing location
    vmm_remap(0xA000000000, 0xA000002000, 3);
    
    printk("OK", "Kernel end");

    for (;;)
    {
        __asm__("hlt");
    }
}
const char *p1 = " _  _   __   __    __  ____  __  ____  _  _     __   ____\n";
const char *p2 = "/ )( \\ / _\\ (  )  (  )(    \\(  )(_  _)( \\/ )   /  \\ / ___)\n";
const char *p3 = "\\ \\/ //    \\/ (_/\\ )(  ) D ( )(   )(   )  /   (  O )\\___ \\ \n";
const char *p4 = " \\__/ \\_/\\_/\\____/(__)(____/(__) (__) (__/     \\__/ (____/\n";

void banner()
{
    printk("main", "Welcome to ValidityOS");
    putc(0x24b8, -1, -1);
    putc('\n', -1, -1);

    debug(false, "%s%s%s%s", p1, p2, p3, p4);

    gfx_set_colors(0xffffff, 0x0);
    printk("Banner", "\n%s%s%s%s", p1, p2, p3, p4);
    gfx_restore_colors(); //Restore default color scheme
    delay(200);
}
