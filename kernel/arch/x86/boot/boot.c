#define PR_MODULE "boot"
#include <arch/x86/hardware/legacy/pic/pic.h>
#include <arch/x86/int/gdt.h>
#include <arch/x86/int/idt.h>
#include <base/assert.h>
#include <base/bootargs.h>
#include <base/mem.h>
#include <kernel.h>
#include <mm/dynamic/kmalloc.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <moon-io/serial.h>
#include <moon.h>
#include <platform.h>
#include <printk.h>
#include "proto/limine.h"
#include "stivale2.h"

void banner(void)
{
    tty_write("Welcome to MoonOS\n");
    tty_write(
        "\n"
        " _____             _____ _____  \n"
        "|     |___ ___ ___|     |   __| \n"
        "| | | | . | . |   |  |  |__   | \n"
        "|_|_|_|___|___|_|_|_____|_____| \n"
        "\n");
}

ALIGN_SECTION(16) static uint8_t stack[8192];

void boot(void)
{
    BootHandover handover = limine_initialize();

    set_hhdm_offset(handover.hhdm_offset);
    printk_init(true, handover.framebuffer);

    trace(TRACE_BOOT, "Reached target pmm\n");
    {
        init_phys_allocator(handover.memory_map);
    }

    trace(TRACE_BOOT, "Reached target vmm\n");
    {
        extern void arch_init_paging(BootHandover);
        arch_init_paging(handover);
    }

    trace(TRACE_BOOT, "Reached target gdt and tss\n");
    init_gdt((uint64_t)stack + sizeof((uint64_t)stack));

    // Core#0 will remap the pic once.
    // After acpi_init the pic is disabled in favor of the apic
    trace(TRACE_BOOT, "Reached target pic\n");
    pic_remap();
    init_idt();

    banner();

    kalloc_init();
    platform_init(handover);
    init_percpu((uint64_t)stack);
    idt_set_entry(KRNL_CS64, 0, 0x8E, (uint64_t)isr32, 32);
    kern_main(handover.modules);
}
