#ifndef GDT_H
#define GDT_H

#include <ktypes.h>
#include <util/common.h>
#include <amd64/moon.h>

#define KRNL_CS64 0x08UL
#define KRNL_DS64 0x10UL
#define USER_DS64 0x18UL
#define USER_CS64 0x20UL

$packed_struct(tss_descriptor, {
	uint16_t size;
	uint16_t base0;
	uint8_t base1;
	uint8_t access;
	uint8_t flags;
	uint8_t base2;
	uint32_t base3;
	uint32_t reserved;
});

$packed_struct(tss_t, {
	uint32_t reserved0;
	uint64_t RSP0;
	uint64_t RSP1;
	uint64_t RSP2;

	uint32_t reserved1;
	uint32_t reserved2;

	uint64_t IST1;
	uint64_t IST2;
	uint64_t IST3;
	uint64_t IST4;
	uint64_t IST5;
	uint64_t IST6;
	uint64_t IST7;

	uint32_t reserved3;
	uint32_t reserved4;

	uint16_t IOBP;
});

struct memory_segment
{
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t gran;
	uint8_t base_high;
} PACKED;

struct gdtr
{
	uint16_t limit;
	uint64_t offset;
} PACKED;

struct gdt_table
{
	struct memory_segment gdt_table_memory_segments[5];
	tss_descriptor tssd;
	struct gdtr gdtr;
} PACKED;

void init_gdt(uint64_t stack);
void init_tss(uint64_t stack);
void set_tss_entry(uint64_t base, uint8_t flags, uint8_t access);

inline void load_tss(uint16_t tss_selector)
{
	asm volatile("ltr %%ax" ::"a"(tss_selector)
				 : "memory");
}

#endif // GDT_H