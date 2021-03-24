#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "stivale2.h"
#include "boot/bootloader_stivale2.h"

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

#include "common.h"

void kmain(struct stivale2_struct *stivale2_struct) {
    vga_init(VGA_WHITE, VGA_BLACK);

    ASM_x86_cpuid_vendor_string();

    init_gdt();
    init_idt();

    //This doesn't work as expected. See boot/bootloader_stivale2.c/h for more
//    bootloader_stivale2_init_mmap(stivale2_struct);

    for (;;) {
        asm ("hlt");
    }
}
