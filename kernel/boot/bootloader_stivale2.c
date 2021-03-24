#include "bootloader_stivale2.h"
#include "../drivers/vga/vga.h"
#include "../stivale2.h"
#include <stdint.h>

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

static uint8_t stack[4096];

struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag = {
        // Identification constant defined in stivale2.h and the specification.
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        // If next is 0, then this marks the end of the linked list of tags.
        .next = 0
    },
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

// How do I do this?
// struct stivale2_header_tag_smp smp = {
//     .tag = {
//         .identifier = STIVALE2_HEADER_TAG_SMP_ID,
//         .next = 0
//     },
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
    .tags = 0
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


void bootloader_stivale2_init_mmap(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    debug("mmap_base: %x", memory_map->memmap->type); //Invalid Opcode??
}

void bootloader_stivale2_init_smp(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_smp_info *s;
    s = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_SMP_ID);
 
    debug("%d\n", s->processor_id);
}