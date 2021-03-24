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

void kmain(boot_info_t *bootvars) {
    vga_init(VGA_WHITE, VGA_BLACK);

//    ASM_x86_cpuid_vendor_string(); //Calling this function will clear out every variable in bootvars. Find out why.

    init_gdt();
    init_idt();

    debug("%d\n", bootvars->cpu.processor_count);

    for (;;) {
        asm ("hlt");
    }
}


