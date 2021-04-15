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

#include "drivers/vga/vga.h"
#include "drivers/io/serial.h"

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
#include "mm/linear_alloc.h"

/*
	TODO:
		- Add a ctype.h/c file to libk for functions like isdigit and co.
*/

void kmain(boot_info_t *bootvars) {
    init_gdt();
    init_idt();

    ASM_x86_cpuid_vendor_string();

    //Safely create a memory buffer of 10 bytes and allocate 4 bytes
    if (linear_mm_init((void*)0, sizeof(char) * 10) == 0) {
        linear_alloc(4, 0);
    }
    linear_mm_release_buffer();

    //Bitmap example usage (this is not a mm, just a bitmap)
    uint64_t *bitmap_arena;
    liballoc_bitmap_t bitmap_object = bitmap_init(bitmap_arena, 20);
    
    bitmap_log_all_bits(bitmap_object);

    //Clear all bits
    bitmap_purge(bitmap_object);

    //Set the 10th bit in the bitmap
    bitmap_object.set(bitmap_object.pool, 20/2);

    //Get all bits
    bitmap_log_all_bits(bitmap_object);

    for (;;) {
        asm ("hlt");
    }
}
