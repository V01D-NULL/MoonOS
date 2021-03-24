#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "../common.h"
#include "../drivers/vga/vga.h"

struct memory_segment {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_high;
}__attribute__((packed));

struct gdtr {
    uint16_t limit;
    uint64_t offset;
}__attribute__((packed));

struct gdt_table {
    struct memory_segment gdt_table_memory_segments[5];
    //tss
    struct gdtr gdtr;
}__attribute__((packed));

void init_gdt();

#endif // GDT_H