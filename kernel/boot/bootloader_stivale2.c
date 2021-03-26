#include "bootloader_stivale2.h"
#include "../drivers/vga/vga.h"
#include "../stivale2.h"
#include "../kernel.h"
#include <stdint.h>

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

static uint8_t stack[4096];

struct stivale2_header_tag_smp smp = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = 0
    },
};

// struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
//     // All tags need to begin with an identifier and a pointer to the next tag.
//     .tag = {
//         .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
//         .next = (uintptr_t)&smp
//     },
//     .framebuffer_width  = 0,
//     .framebuffer_height = 0,
//     .framebuffer_bpp    = 0
// };

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header stivale_hdr = {
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0,
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // No flags are currently defined as per spec and should be left to 0.
    .flags = 0,
    // This header structure is the root of the linked list of header tags and
    // points to the first one (and in our case, only).
    .tags = (uintptr_t)&smp
};

//Stolen from the limine barebones tutorial (I don't feel like re-inventing the wheel rn)
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


void kinit(struct stivale2_struct *bootloader_info) {
    boot_info_t bootvars; //Hardware information from the bootloader

    serial_set_color(BASH_WHITE);

    //Vesa support will come later on (probably after memory management)
    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
    struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (fb != NULL) {

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
        //Todo: Caluclate total memory from this memory map and save it in bootvars.mmap.ram
    }

    kmain(&bootvars);
}
