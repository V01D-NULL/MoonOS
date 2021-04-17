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

    ASM_x86_cpuid_vendor_string();

    for (;;) {
        asm ("hlt");
    }
}
