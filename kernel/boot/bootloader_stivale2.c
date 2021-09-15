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
#include <drivers/io/serial.h>
#include <drivers/gfx/gfx.h>
#include <amd64/validity.h>
#include <stivale2.h>
#include <int/idt.h>
#include <int/gdt.h>
#include <kernel.h>
#include <mm/pmm.h>
#include <stdint.h>

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

static __section_align uint8_t stack[345859];

struct stivale2_header_tag_smp smp_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = 0
    },
};

struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&smp_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

#ifdef USE_VGA
__SECTION(".stivale2hdr")
struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 0,
    .tags = 0//(uintptr_t)&framebuffer_hdr_tag
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

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        
        if (current_tag == NULL) {
            return NULL;
        }

        
        if (current_tag->identifier == id) {
            return current_tag;
        }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (void *)current_tag->next;
    }
}

/**
 * @brief Kernel entry point
 * 
 * @param[in] bootloader_info Various information from the limine bootloader
 */
void kinit(struct stivale2_struct *bootloader_info) {
    boot_info_t bootvars; //Hardware information from the bootloader

    serial_set_color(BASH_WHITE);
    
    #ifdef USE_VGA
    vga_init(0xff, 0x0);
    #endif

    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
    struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    

    if (fb != NULL) {
        bootvars.vesa.fb_addr = fb->framebuffer_addr;
        bootvars.vesa.fb_width = fb->framebuffer_width;
        bootvars.vesa.fb_height = fb->framebuffer_height;
        bootvars.vesa.fb_bpp = fb->framebuffer_bpp;
        bootvars.vesa.fb_pitch = fb->framebuffer_pitch;
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
        bootvars.mmap.entries = mmap->entries;
        bootvars.mmap.base = mmap->memmap->base;
        bootvars.mmap.length = mmap->memmap->length;
        bootvars.mmap.type = mmap->memmap->type;
        
        for (int i = 0; i < mmap->entries; i++)
        {
            
            bootvars.mmap.memmap[i] = mmap->memmap[i];
            struct stivale2_mmap_entry *internal_mmap = &mmap->memmap[i];
            
            bootvars.mmap.total_ram += internal_mmap->length;

            if (internal_mmap->type == STIVALE2_MMAP_USABLE) //|| internal_mmap->type == STIVALE2_MMAP_ACPI_RECLAIMABLE || internal_mmap->type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
            {
                bootvars.mmap.free_ram += internal_mmap->length;
            }
            else {
                bootvars.mmap.used_ram += internal_mmap->length;
            }
        }
    }

    //It's ugly code but pretty output :)
    debug(false, "Total RAM: ");
    serial_set_color(BASH_CYAN);
    debug(false, "%ld kb\n", bootvars.mmap.total_ram);
    serial_set_color(BASH_WHITE);
    debug(false, "Free  RAM: ");
    serial_set_color(BASH_CYAN);
    debug(false, "%ld kb\n", bootvars.mmap.free_ram);
    serial_set_color(BASH_WHITE);
    debug(false, "Used  RAM: ");
    serial_set_color(BASH_CYAN);
    debug(false, "%ld kb\n", bootvars.mmap.used_ram);
    serial_set_color(BASH_WHITE);

    kmain(&bootvars);
}