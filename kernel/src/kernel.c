#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"

#include "int/gdt.h"
#include "int/idt.h"
#include "int/interrupts.h"

#include "drivers/screen/monitor.h"
#include "drivers/io/serial.h"

#include "hal/apic.h"

#include "asm/x86/x86.h"

#include "libk/kstdlib.h"
#include "libk/kstring.h"

//Specifies the stack required by stivale2. Stored in .bss
static uint8_t stack[4096];

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 0,
};

void kmain(struct stivale2_struct *stivale2_struct) {
    ASM_x86_cpuid_vendor_string();

    init_gdt();

    init_idt();
    
    debug("Improved debug() demo: %d %d 0x%x %b\n", -120, 120, 120, 120);
    kprintf("Improved kprintf() demo: %d %d 0x%x %b\n", -120, 120, 120, 120);

    for (;;) {
        asm ("hlt");
    }
}
