#include "gdt.h"

extern void _load_gdt(uint64_t gdt_desc);

static struct gdt_table gdt;

void init_gdt()
{
    vga_puts("Initialising GDT\n", true, false);
    
    //Null descriptor
    gdt.gdt_table_memory_segments[0].limit     = 0;
    gdt.gdt_table_memory_segments[0].base_low  = 0;
    gdt.gdt_table_memory_segments[0].base_mid  = 0;
    gdt.gdt_table_memory_segments[0].access    = 0;
    gdt.gdt_table_memory_segments[0].gran      = 0;
    gdt.gdt_table_memory_segments[0].base_high = 0;

    //Kernel CS
    gdt.gdt_table_memory_segments[1].limit = 0;
    gdt.gdt_table_memory_segments[1].base_low = 0;
    gdt.gdt_table_memory_segments[1].base_mid = 0;
    gdt.gdt_table_memory_segments[1].access = 0b10011010;
    gdt.gdt_table_memory_segments[1].gran = 0b00100000;
    gdt.gdt_table_memory_segments[1].base_high = 0;

    // Kernel DS
    gdt.gdt_table_memory_segments[2].limit = 0;
    gdt.gdt_table_memory_segments[2].base_low = 0;
    gdt.gdt_table_memory_segments[2].base_mid = 0;
    gdt.gdt_table_memory_segments[2].access = 0b10010110;
    gdt.gdt_table_memory_segments[2].gran = 0;
    gdt.gdt_table_memory_segments[2].base_high = 0;

    // User DS
    gdt.gdt_table_memory_segments[3].limit = 0;
    gdt.gdt_table_memory_segments[3].base_low = 0;
    gdt.gdt_table_memory_segments[3].base_mid = 0;
    gdt.gdt_table_memory_segments[3].access = 0b11110010;
    gdt.gdt_table_memory_segments[3].gran = 0;
    gdt.gdt_table_memory_segments[3].base_high = 0;

    // user code 64
    gdt.gdt_table_memory_segments[4].limit = 0;
    gdt.gdt_table_memory_segments[4].base_low = 0;
    gdt.gdt_table_memory_segments[4].base_mid = 0;
    gdt.gdt_table_memory_segments[4].access = 0b11111010;
    gdt.gdt_table_memory_segments[4].gran = 0b00100000;

    gdt.gdtr.offset = (uint64_t)&gdt.gdt_table_memory_segments;
    gdt.gdtr.limit = sizeof(struct memory_segment) * 5 - 1;

    _load_gdt((uint64_t)&gdt.gdtr);
    
    vga_puts("Initialised GDT\n", false, true);
}
