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

#include "stivale2.h"
#include "boot/bootloader_stivale2.h"

#include "amd64/cpu.h"

#include "int/gdt.h"
#include "int/idt.h"
#include "int/interrupts.h"

#include "drivers/io/serial.h"
#include "drivers/gfx/gfx.h"

#include "hal/apic.h"

#include "asm/x86/x86.h"

//Defined in libs/libk/
#include "libk/kstring.h"
#include "libk/kprintf.h"
#include "libk/kassert.h"
#include "liballoc/bitmap.h"

#include "common.h"

#include "mm/ram.h"
#include "mm/pmm.h"
#include "mm/paging/pfa.h"
#include "mm/linear_alloc.h"

/*
	TODO:
		- Add a ctype.h/c file to libk for functions like isdigit and co.
*/

bool test_wait()
{
    delay(500);
    return true;
}

void banner();
extern liballoc_bitmap_t bitmap_manager;

void kmain(boot_info_t *bootvars) {
    /* Init the VESA printing routines, font loading, etc */
    gfx_init(*bootvars, 0xffffff, 0x0);

    /* Init the CPU hardware struct */
    cpu_info_init(*bootvars);

    banner();

    printk("main", "Test log message\n");
    printk("ERR", "Test error\n");
    printk("OK", "Test succession\n");

    printk_wait(test_wait, "Testing the wait function\n");
    
    x86_cpu_info();

    init_pmm(bootvars->mmap.memmap, bootvars->mmap.entries);

    //TODO: 
    //Create a function that converts the bit offset in the bitmap to a physical address / address of the page
    
    //PMM Demo
    int32_t offset = pmm_alloc();
    bitmap_log_all_bits(bitmap_manager);
    pmm_free(offset);
    bitmap_log_all_bits(bitmap_manager);

    for (;;) {
        asm ("hlt");
    }
}

const char* p1 = " _  _   __   __    __  ____  __  ____  _  _     __   ____\n";
const char* p2 = "/ )( \\ / _\\ (  )  (  )(    \\(  )(_  _)( \\/ )   /  \\ / ___)\n";
const char* p3 = "\\ \\/ //    \\/ (_/\\ )(  ) D ( )(   )(   )  /   (  O )\\___ \\ \n";
const char* p4 = " \\__/ \\_/\\_/\\____/(__)(____/(__) (__) (__/     \\__/ (____/\n";

void banner() {
    printk("main", "Welcome to ValidityOS");
    putc(0x24b8, -1, -1);
    putc('\n', -1, -1);

    debug("%s%s%s%s", p1, p2, p3, p4);

    gfx_set_colors(0x4863A0, 0x0); //Dark/Dirty blue on black bg
    printk("", "%s", p1);
    gfx_set_colors(0x6698FF, 0x0); //Light blue on black bg
    printk("", "%s", p2);
    gfx_set_colors(0x6960EC, 0x0); //Dark Purple on black bg
    printk("", "%s", p3);
    gfx_set_colors(0x4863A0, 0x0); //Dark/Dirty blue on black bg
    printk("", "%s", p4);
    
    gfx_restore_colors(); //Restore default color scheme
    delay(200);
}