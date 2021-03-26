#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "stivale2.h"
#include "boot/bootloader_stivale2.h"

#include "AMD/cpu.h"

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

#include "mm/ram.h"
#include "mm/linear_alloc.h"

//TODO:
//  Create a mm/ram.h/c file where Validity will keep track of RAM usage (the struct will be updated with each memory allocation and deallocation)
//
void kmain(boot_info_t *bootvars) {
    ram_manager_init(*bootvars);
    
    init_gdt();
    init_idt();
    
    ASM_x86_cpuid_vendor_string();

    //Safely create a memory buffer of 10 bytes and allocate 4 bytes
    if (linear_mm_init((void*)0, sizeof(char) * 10) == 0) {
        linear_alloc(4);
    }
    linear_mm_release_buffer();
    
    //Invalid use of linear_alloc() example A Linear allocation pool must be initialised first
    linear_alloc(20);

    for (;;) {
        asm ("hlt");
    }
}
