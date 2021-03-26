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
#include "libk/kassert.h"

#include "common.h"

//TODO:
//  Create a mm/ram.h/c file where Validity will keep track of RAM usage (the struct will be updated with each memory allocation and deallocation)
//
void kmain(boot_info_t *bootvars) {
    
    ASM_x86_cpuid_vendor_string();

    init_gdt();
    init_idt();

    debug("%d\n", bootvars->cpu.processor_count);
    debug("rip: 0x%x\n", ASM_x86_regs_read_rip());

    assert(1 == 0);

    for (;;) {
        asm ("hlt");
    }
}
