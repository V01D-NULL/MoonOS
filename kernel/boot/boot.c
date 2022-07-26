#include <devices/term/early/early_term.h>
#include <devices/term/limine-port/term.h>
#include <libgraphics/bootsplash_img.h>
#include <boot/proto/proto-stivale2.h>
#include <devices/serial/serial.h>
#include <devices/fb/early_fb.h>
#include <libgraphics/draw.h>
#include <hal/acpi/acpi.h>
#include <hal/apic/apic.h>
#include <libk/cmdline.h>
#include <hal/pic/pic.h>
#include <amd64/moon.h>
#include <stivale2.h>
#include <int/idt.h>
#include <int/gdt.h>
#include <kernel.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/mm.h>
#include <printk.h>
#include "boot.h"

const char serial_message[] = {
    "███╗   ███╗ ██████╗  ██████╗ ███╗   ██╗ ██████╗ ███████╗ \n"
    "████╗ ████║██╔═══██╗██╔═══██╗████╗  ██║██╔═══██╗██╔════╝ \n"
    "██╔████╔██║██║   ██║██║   ██║██╔██╗ ██║██║   ██║███████╗ \n"
    "██║╚██╔╝██║██║   ██║██║   ██║██║╚██╗██║██║   ██║╚════██║ \n"
    "██║ ╚═╝ ██║╚██████╔╝╚██████╔╝██║ ╚████║╚██████╔╝███████║ \n"
    "╚═╝     ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚══════╝ \n"};

const char fb_message[] = {
    "\n"
    " _____             _____ _____  \n"
    "|     |___ ___ ___|     |   __| \n"
    "| | | | . | . |   |  |  |__   | \n"
    "|_|_|_|___|___|_|_|_____|_____| \n"
    "\n"};

void banner(bool serial_only)
{
    if (serial_only)
    {
        debug(false, "%s", serial_message);
        return;
    }

    fmt_puts("Welcome to MoonOS\n");
    fmt_puts("%s", fb_message);
}

EXTERNAL(stack);
static BootContext ctx;

void kinit(struct stivale2_struct *bootloader_info)
{
    ctx.rbp = (uint64_t)stack;
    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    struct stivale2_struct_tag_terminal *term = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
    struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
    struct stivale2_struct_tag_cmdline *cmdline = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_CMDLINE_ID);
    struct stivale2_struct_tag_modules *modules = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MODULES_ID);
    struct stivale2_struct_tag_firmware *fw = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FIRMWARE_ID);
    ctx.is_uefi = !(fw->flags & STIVALE2_FIRMWARE_BIOS);

    if (!term)
    {
        set_boot_term_available(false);
    }
    else
    {
        boot_term_init(term->term_write, term->cols, term->rows);
    }

    // Can't work under these conditions
    if (!fb)
    {
        debug(false, BASH_RED "WARNING: " BASH_DEFAULT "MoonOS was NOT provided with a framebuffer. Refusing to boot.");
        for (;;)
            ;
    }
    ctx.fb.fb_addr = fb->framebuffer_addr;
    ctx.fb.fb_width = fb->framebuffer_width;
    ctx.fb.fb_height = fb->framebuffer_height;
    ctx.fb.fb_bpp = fb->framebuffer_bpp;
    ctx.fb.fb_pitch = fb->framebuffer_pitch;

    if (smp != NULL)
    {
        ctx.cpu.processor_count = smp->cpu_count;
        ctx.cpu.bootstrap_processor_lapic_id = smp->bsp_lapic_id;
        ctx.cpu.acpi_processor_uid = smp->smp_info->processor_id;
        ctx.cpu.lapic_id = smp->smp_info->lapic_id;
        ctx.cpu.smp_info = smp->smp_info;
    }

    if (rsdp != NULL)
    {
        ctx.rsdp.rsdp_address = rsdp->rsdp;
    }

    if (mmap != NULL)
    {
        boot_term_write("boot: Copying memory map to boot context\n");
        memcpy((uint8_t *)ctx.mmap, (uint8_t *)mmap, sizeof(struct stivale2_struct_tag_memmap) * mmap->entries);

        // Prepare the terminal
        term_prepare(fb, mmap);
        
        boot_term_write("boot: Reached target pmm\n");
        pmm_init(mmap->memmap, mmap->entries);

        boot_term_write("boot: Reached target vmm\n");
        v_init(mmap->memmap, mmap->entries);
        
		boot_term_write("boot: Reached target gdt and tss\n");
        init_gdt((uint64_t)stack + sizeof((uint64_t)stack)); // Note: boot_term_write is unusable now

        // Core#0 will remap the pic once.
        // After acpi_init the pic is disabled in favor of the apic
        pic_remap();
        init_idt();

        /* Is verbose boot specified in the command line? */
        if (cmdline != NULL)
        {
            if (!boot_cmdline_find_tag("verbose_boot", (const char *)cmdline->cmdline))
            {
                early_fb_init(ctx);
                fb_draw_image((ctx.fb.fb_width / 2) - (IMG_WIDTH / 2), (ctx.fb.fb_height / 2) - (IMG_HEIGHT / 2), IMG_WIDTH, IMG_HEIGHT, IMG_DATA, IMAGE_RGB);
                printk_init(false, ctx);
            }
            else
            {
                printk_init(true, ctx);
            }
        }
        else
        {
            for (;;)
                ;
        }

        banner(false);
    }
    else
    {
        debug(false, "\n!Did not get a memory map from the bootloader!\n");
        boot_term_write("Fatal: Cannot obtain a memory map from the bootloader");
        for (;;)
            ;
    }

    kmain(&ctx, modules);
}

BootContext BootContextGet(void)
{
    return ctx;
}