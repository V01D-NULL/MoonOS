#include "gdt.h"

gnu_export void _load_gdt(uint64_t gdt_desc);

static struct gdt_table gdt;

void init_gdt(void)
{
    //Null descriptor
    gdt.gdt_table_memory_segments[0].limit     = 0;
    gdt.gdt_table_memory_segments[0].base_low  = 0;
    gdt.gdt_table_memory_segments[0].base_mid  = 0;
    gdt.gdt_table_memory_segments[0].access    = 0;
    gdt.gdt_table_memory_segments[0].gran      = 0;
    gdt.gdt_table_memory_segments[0].base_high = 0;

    // These GDT descriptors are mandatory in order to use the stivale2 terminal

    // 16 bit kernel CS
    gdt.gdt_table_memory_segments[1].limit = 0xffff;
    gdt.gdt_table_memory_segments[1].base_low = 0;
    gdt.gdt_table_memory_segments[1].base_mid = 0;
    gdt.gdt_table_memory_segments[1].access = 0x9A;
    gdt.gdt_table_memory_segments[1].gran   = 0x80;
    gdt.gdt_table_memory_segments[1].base_high = 0;

    // 16 bit kernel DS
    gdt.gdt_table_memory_segments[2].limit = 0xffff;
    gdt.gdt_table_memory_segments[2].base_low = 0;
    gdt.gdt_table_memory_segments[2].base_mid = 0;
    gdt.gdt_table_memory_segments[2].access = 0x9A;
    gdt.gdt_table_memory_segments[2].gran = 0x80;
    gdt.gdt_table_memory_segments[2].base_high = 0;

    // 32 bit kernel CS
    gdt.gdt_table_memory_segments[3].limit = 0xffff;
    gdt.gdt_table_memory_segments[3].base_low = 0;
    gdt.gdt_table_memory_segments[3].base_mid = 0;
    gdt.gdt_table_memory_segments[3].access = 0x9A;
    gdt.gdt_table_memory_segments[3].gran = 0xCF;
    gdt.gdt_table_memory_segments[3].base_high = 0;

    // 32 bit kernel DS
    gdt.gdt_table_memory_segments[4].limit = 0xffff;
    gdt.gdt_table_memory_segments[4].base_low = 0;
    gdt.gdt_table_memory_segments[4].base_mid = 0;
    gdt.gdt_table_memory_segments[4].access = 0x92;
    gdt.gdt_table_memory_segments[4].gran = 0xCF;
    gdt.gdt_table_memory_segments[4].base_high = 0;

    // 64 bit kernel CS
    gdt.gdt_table_memory_segments[5].limit = 0;
    gdt.gdt_table_memory_segments[5].base_low = 0;
    gdt.gdt_table_memory_segments[5].base_mid = 0;
    gdt.gdt_table_memory_segments[5].access = 0x9A;
    gdt.gdt_table_memory_segments[5].gran = 0xA2;
    gdt.gdt_table_memory_segments[5].base_high = 0;
    
    // 64 bit kernel DS
    gdt.gdt_table_memory_segments[6].limit = 0;
    gdt.gdt_table_memory_segments[6].base_low = 0;
    gdt.gdt_table_memory_segments[6].base_mid = 0;
    gdt.gdt_table_memory_segments[6].access = 0x92;
    gdt.gdt_table_memory_segments[6].gran = 0xA0;
    gdt.gdt_table_memory_segments[6].base_high = 0;

    // 64 bit user DS
    gdt.gdt_table_memory_segments[7].limit = 0;
    gdt.gdt_table_memory_segments[7].base_low = 0;
    gdt.gdt_table_memory_segments[7].base_mid = 0;
    gdt.gdt_table_memory_segments[7].access = 0xF2;
    gdt.gdt_table_memory_segments[7].gran = 0;
    gdt.gdt_table_memory_segments[7].base_high = 0;

    // 64 bit user CS
    gdt.gdt_table_memory_segments[8].limit = 0;
    gdt.gdt_table_memory_segments[8].base_low = 0;
    gdt.gdt_table_memory_segments[8].base_mid = 0;
    gdt.gdt_table_memory_segments[8].access = 0xFA;
    gdt.gdt_table_memory_segments[8].gran = 0x20;

    gdt.gdtr.offset = (uint64_t)&gdt.gdt_table_memory_segments;
    gdt.gdtr.limit = sizeof(struct memory_segment) * 13 - 1;

    _load_gdt((uint64_t)&gdt.gdtr);
}
