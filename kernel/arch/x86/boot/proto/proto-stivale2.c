#include "proto-stivale2.h"
#include <moon.h>

ALIGN_SECTION(16) uint8_t stack[8192];

struct stivale2_struct_tag_cmdline cmdline_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_CMDLINE_ID,
        .next = 0}};

struct stivale2_struct_tag_rsdp rsdp_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
        .next = (uintptr_t)&cmdline_tag}};

struct stivale2_tag level5_paging_tag = {
    .identifier = STIVALE2_HEADER_TAG_5LV_PAGING_ID,
    .next = (uintptr_t)&rsdp_tag};

struct stivale2_header_tag_smp smp_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = (uintptr_t)&level5_paging_tag}};

struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&smp_hdr_tag},
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32};

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = (uintptr_t)&framebuffer_hdr_tag},
    .flags = 0};

SECTION(".stivale2hdr")
struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 4),
    .tags = (uintptr_t)&terminal_hdr_tag};

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