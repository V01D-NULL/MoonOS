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

void banner();

void kmain(boot_info_t *bootvars) {
    vga_init(VGA_LIGHT_GREY, VGA_BLACK);
    
    banner();
    
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

const char* p1 = " _  _   __   __    __  ____  __  ____  _  _     __   ____\n";
const char* p2 = "/ )( \\ / _\\ (  )  (  )(    \\(  )(_  _)( \\/ )   /  \\ / ___)\n";
const char* p3 = "\\ \\/ //    \\/ (_/\\ )(  ) D ( )(   )(   )  /   (  O )\\___ \\ \n";
const char* p4 = " \\__/ \\_/\\_/\\____/(__)(____/(__) (__) (__/     \\__/ (____/\n";

void banner() {
    int bg = vga_hist.cell_color_bg;
    int fg = vga_hist.cell_color_fg;

    debug("%s%s%s%s", p1, p2, p3, p4);

    set_color(VGA_BLACK, VGA_DARK_GREY);
    kprintf("%s", p1);
    set_color(VGA_BLACK, VGA_LIGHT_GREY);
    kprintf("%s", p2);
    set_color(VGA_BLACK, VGA_WHITE);
    kprintf("%s", p3);
    set_color(VGA_BLACK, VGA_LIGHT_BLUE);
    kprintf("%s", p4);
    set_color(bg, fg);
    delay(200);
}