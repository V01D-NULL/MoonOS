/**
 * @file bootloader_stivale2.c
 * @author Tim (V01D)
 * @brief Hanldes information from the bootloader
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "bootloader_stivale2.h"
#include <drivers/vbe/vbe.h>
#include <drivers/io/serial.h>
#include <amd64/validity.h>
#include <stivale2.h>
#include <int/idt.h>
#include <int/gdt.h>
#include <kernel.h>
#include <mm/pmm.h>
#include <panic.h>
#include <stdint.h>

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

static __section_align uint8_t stack[345859];

struct stivale2_struct_tag_rsdp rsdp_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
        .next = 0
    }
};

struct stivale2_tag level5_paging_tag = {
    .identifier = STIVALE2_HEADER_TAG_5LV_PAGING_ID,
    .next = (uintptr_t)&rsdp_tag
};

struct stivale2_header_tag_smp smp_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = (uintptr_t)&level5_paging_tag
    }
};

struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&smp_hdr_tag
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

#ifdef USE_VGA
__SECTION(".stivale2hdr")
struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 0,
    .tags = 0 //(uintptr_t)&framebuffer_hdr_tag
};
#else
__SECTION(".stivale2hdr")
struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 0,
    .tags = (uintptr_t)&framebuffer_hdr_tag
};
#endif

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;)
    {

        if (current_tag == NULL)
        {
            return NULL;
        }

        if (current_tag->identifier == id)
        {
            return current_tag;
        }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (void *)current_tag->next;
    }
}

const char *p1 = " _  _   __   __    __  ____  __  ____  _  _     __   ____\n";
const char *p2 = "/ )( \\ / _\\ (  )  (  )(    \\(  )(_  _)( \\/ )   /  \\ / ___)\n";
const char *p3 = "\\ \\/ //    \\/ (_/\\ )(  ) D ( )(   )(   )  /   (  O )\\___ \\ \n";
const char *p4 = " \\__/ \\_/\\_/\\____/(__)(____/(__) (__) (__/     \\__/ (____/\n";

void banner()
{
    printk("main", "Welcome to ValidityOS");
    putc(0x24b8, -1, -1);
    putc('\n', -1, -1);

    debug(false, "%s%s%s%s", p1, p2, p3, p4);

    gfx_set_colors(0x4863A0, 0x0); //Dark/Dirty blue on black bg
    printk("", "%s", p1);
    gfx_set_colors(0x6698FF, 0x0); //Light blue on black bg
    printk("", "%s", p2);
    gfx_set_colors(0x6960EC, 0x0); //Dark Purple on black bg
    printk("", "%s", p3);
    gfx_set_colors(0x4863A0, 0x0); //Dark/Dirty blue on black bg
    printk("", "%s", p4);
    
    gfx_restore_colors(); //Restore default color scheme
    delay(200);
}

/**
 * @brief Kernel entry point
 * 
 * @param[in] bootloader_info Various information from the limine bootloader
 */
void kinit(struct stivale2_struct *bootloader_info)
{
    boot_info_t bootvars; //Hardware information from the bootloader

    serial_set_color(BASH_WHITE);

#ifdef USE_VGA
    vga_init(0xff, 0x0);
#else
    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
#endif

    struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
    struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);

    init_gdt();
    init_idt();

    if (fb != NULL)
    {
        bootvars.vesa.fb_addr = fb->framebuffer_addr;
        bootvars.vesa.fb_width = fb->framebuffer_width;
        bootvars.vesa.fb_height = fb->framebuffer_height;
        bootvars.vesa.fb_bpp = fb->framebuffer_bpp;
        bootvars.vesa.fb_pitch = fb->framebuffer_pitch;

        /* Init the VESA printing routines, font loading, etc */
        gfx_init(bootvars, 0xffffff, 0x00);
        banner();
    }

    if (smp != NULL)
    {
        bootvars.cpu.processor_count = smp->cpu_count;
        bootvars.cpu.bootstrap_processor_lapic_id = smp->bsp_lapic_id;
        bootvars.cpu.acpi_processor_uid = smp->smp_info->processor_id;
        bootvars.cpu.lapic_id = smp->smp_info->lapic_id;
    }

    if (mmap != NULL)
    {
        pmm_init(mmap->memmap, mmap->entries);
    }
    else
    {
        panic("Did not get a memory map from the bootloader");
    }

    if (rsdp != NULL)
    {
        bootvars.rsdp.rsdp_address = rsdp->rsdp;
    }

    kmain(&bootvars);
}