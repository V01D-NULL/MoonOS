#include "gdt.h"
#include <base/string.h>

extern void load_gdt(uint64_t gdtr);

static struct __attribute__((align(64))) gdt_table gdt;

void set_tss_entry(uint64_t base, uint8_t flags, uint8_t access)
{
    gdt.tssd.size = 104;
    gdt.tssd.base0 = base & 0xFFFF;
    gdt.tssd.base1 = (base >> 16) & 0xFF;
    gdt.tssd.access = access;
    gdt.tssd.flags = flags;
    gdt.tssd.base2 = (base >> 24) & 0xFF;
    gdt.tssd.base3 = (base >> 32);
    gdt.tssd.reserved = 0;
}

static tss_t tss;
void init_tss(uint64_t stack)
{
    set_tss_entry((uintptr_t)&tss, 0x20, 0x89);
    memset((void *)&tss, 0, sizeof(tss_t));

    tss.RSP0 = stack;
    tss.IST1 = 0; // Disable IST
}

void init_gdt(uint64_t stack)
{
    // Null descriptor
    gdt.gdt_table_memory_segments[0].limit = 0;
    gdt.gdt_table_memory_segments[0].base_low = 0;
    gdt.gdt_table_memory_segments[0].base_mid = 0;
    gdt.gdt_table_memory_segments[0].access = 0;
    gdt.gdt_table_memory_segments[0].gran = 0;
    gdt.gdt_table_memory_segments[0].base_high = 0;

    // 64 bit kernel CS
    gdt.gdt_table_memory_segments[1].limit = 0;
    gdt.gdt_table_memory_segments[1].base_low = 0;
    gdt.gdt_table_memory_segments[1].base_mid = 0;
    gdt.gdt_table_memory_segments[1].access = 0x9A;
    gdt.gdt_table_memory_segments[1].gran = 0xA2;
    gdt.gdt_table_memory_segments[1].base_high = 0;

    // 64 bit kernel DS
    gdt.gdt_table_memory_segments[2].limit = 0;
    gdt.gdt_table_memory_segments[2].base_low = 0;
    gdt.gdt_table_memory_segments[2].base_mid = 0;
    gdt.gdt_table_memory_segments[2].access = 0x92;
    gdt.gdt_table_memory_segments[2].gran = 0xA0;
    gdt.gdt_table_memory_segments[2].base_high = 0;

    // 64 bit user DS
    gdt.gdt_table_memory_segments[3].limit = 0;
    gdt.gdt_table_memory_segments[3].base_low = 0;
    gdt.gdt_table_memory_segments[3].base_mid = 0;
    gdt.gdt_table_memory_segments[3].access = 0xF2;
    gdt.gdt_table_memory_segments[3].gran = 0;
    gdt.gdt_table_memory_segments[3].base_high = 0;

    // 64 bit user CS
    gdt.gdt_table_memory_segments[4].limit = 0;
    gdt.gdt_table_memory_segments[4].base_low = 0;
    gdt.gdt_table_memory_segments[4].base_mid = 0;
    gdt.gdt_table_memory_segments[4].access = 0xFA;
    gdt.gdt_table_memory_segments[4].gran = 0x20;
    gdt.gdt_table_memory_segments[4].base_high = 0;

    init_tss(stack);

    gdt.gdtr.offset = (uint64_t)&gdt.gdt_table_memory_segments;
    gdt.gdtr.limit = sizeof(struct gdt_table) * 6 - 1;

    load_gdt((uint64_t)&gdt.gdtr);
    load_tss(0x28);
}