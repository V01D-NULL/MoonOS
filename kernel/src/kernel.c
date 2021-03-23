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

    debug("%s - %c - %d - %x - %b", "heyo", 'a', 10, 10, 10);

    char *bin = itob(27); //11011
    // debug("bin: %s\n", bin);

    //TODO:
    //NOTE: common.c/common.h may only define types, custom bit manipulation macros and inX/outX functions
    // 1.  Add itob() function
    // 2.  Rewrite debug and kprintf to use the itob() function
    // 2.1 Remove to_dec, to_bin & decToHex. They are error prone and bloat the kernel. They will all be replaced by itob
    // 3.  Cleanup common.h/common.c | Create lib/string.h etc
    
    for (;;) {
        asm ("hlt");
    }
}
