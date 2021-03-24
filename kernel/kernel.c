#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "boot/bootloader_stivale2.h"

#include "int/gdt.h"
#include "int/idt.h"
#include "int/interrupts.h"

#include "drivers/vga/vga.h"
#include "drivers/io/serial.h"

#include "hal/apic.h"

#include "asm/x86/x86.h"

#include "libk/kstdlib.h"
#include "libk/kstring.h"

#include "common.h"

void kmain(struct stivale2_struct *stivale2_struct) {
    vga_init(VGA_WHITE, VGA_BLACK);

    ASM_x86_cpuid_vendor_string();

    set_color(VGA_BLACK, VGA_WHITE);

    init_gdt();

    init_idt();

    set_color(VGA_RED, VGA_BLACK);
    kprintf_p(false, true, "Color changing example\n");

    debug("testing testing %d\n", 1234);



    // bootloader_stivale2_init_smp(stivale2_struct);

    // debug("%d\n", stivale2_struct->tags);

//    x86_cpu_check_apic();

  //  struct stivale2_struct_tag_framebuffer *fb_hdr_tag;
//    fb_hdr_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    
    
    //debug("Framebuffer width: %d\nFramebuffer Height: %d\n", fb_hdr_tag->framebuffer_width, fb_hdr_tag->framebuffer_height);

    //debug("mmap_base: %d\n", mmap->base);

    //debug("Improved debug() demo: %d %d 0x%x %b\n", -120, 120, 120, 120);
    //kprintf("Improved kprintf() demo: %d %d 0x%x %b\n", -120, 120, 120, 120);

    for (;;) {
        asm ("hlt");
    }
}
