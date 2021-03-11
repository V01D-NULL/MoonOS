#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"

#include "int/gdt.h"
#include "int/idt.h"

#include "drivers/screen/monitor.h"
#include "drivers/io/serial.h"

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static uint8_t stack[4096];

// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// As an example header tag, we're gonna define a framebuffer header tag.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode.
// struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
//     // All tags need to begin with an identifier and a pointer to the next tag.
//     .tag = {
//         // Identification constant defined in stivale2.h and the specification.
//         .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
//         // If next is 0, then this marks the end of the linked list of tags.
//         .next = 0
//     },
//     // We set all the framebuffer specifics to 0 as we want the bootloader
//     // to pick the best it can.
//     .framebuffer_width  = 0,
//     .framebuffer_height = 0,
//     .framebuffer_bpp    = 0
// };

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
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
    // .tags = (uintptr_t)&framebuffer_hdr_tag
};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        // If the tag pointer is NULL (end of linked list), we did not find
        // the tag. Return NULL to signal this.
        if (current_tag == NULL) {
            return NULL;
        }

        // Check whether the identifier matches. If it does, return a pointer
        // to the matching tag.
        if (current_tag->identifier == id) {
            return current_tag;
        }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (void *)current_tag->next;
    }
}

// The following will be our kernel's entry point.
void kmain(struct stivale2_struct *stivale2_struct) {
    
    init_gdt();    

    init_idt();

    //Test CPU exception (invalid opcode)
    kprintf("Triggering CPU exception.... %d\n", 0/0);

    // asm volatile("int $2"); //NMI
    
    for (;;) {
        asm ("hlt");
    }
}
