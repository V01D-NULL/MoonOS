#include "descriptorTables.h"

/*
	GDT
		. global descriptor table
		. something about segmentation
		. segmentation obsolete so going to try and setup a flat memory model.
		  Segments window should start at 0x00000000 and extend to 0xFFFFFFFF (the end of memory)
		. can set ring (privilege) level of process
		  0 most privileged, 3 least
		  0 said to be running in kernel/supervisor mode and can use instructions
		  like 'sti' and 'cli'

		. In x86, there are six segmentation registers:
		  . code segment (cs)
		  . data segment (ds)
		  . extra segment (es)
		  . fs
		  . gs
		  . stack segment (ss)

	IDT
		. interrupt vector table
		. array of entries each corresponding to an interrupt number
		. 256
		. first 32 raised by CPU (to signal kernel)
			 0 - Division by zero exception
			 1 - Debug exception
			 2 - Non maskable interrupt
			 3 - Breakpoint exception
			 4 - 'Into detected overflow'
			 5 - Out of bounds exception
			 6 - Invalid opcode exception
			 7 - No coprocessor exception
			 8 - Double fault                (pushes an error code)
			 9 - Coprocessor segment overrun
			10 - Bad TSS                     (pushes an error code)
			11 - Segment not present         (pushes an error code)
			12 - Stack fault                 (pushes an error code)
			13 - General protection fault    (pushes an error code)
			14 - Page fault                  (pushes an error code)
			15 - Unknown interrupt exception
			16 - Coprocessor fault
			17 - Alignment check exception
			18 - Machine check exception
			19-31 - Reserved

*/

// Lets us access our ASM functions from our C code
extern void gdt_flush ( u32int );
extern void idt_flush ( u32int );

// Internal function prototypes
static void init_gdt ();
static void gdt_set_gate ( s32int, u32int, u32int, u8int, u8int );
static void init_idt ();
static void idt_set_gate ( u8int, u32int, u16int, u8int );

gdt_entry_t gdt_entries [ 5 ];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries [ 256 ];
idt_ptr_t   idt_ptr;

// Initializes the GDT and IDT
void init_descriptor_tables ()
{
	init_gdt();
	init_idt();
}

//
static void init_gdt ()
{
	gdt_ptr.limit = ( sizeof( gdt_entry_t ) * 5 ) - 1;
	gdt_ptr.base  = ( u32int ) &gdt_entries;

	gdt_set_gate( 0, 0,          0,    0,    0 );  // Null segment
	gdt_set_gate( 1, 0, 0xFFFFFFFF, 0x9A, 0xCF );  // Kernel mode code segment
	gdt_set_gate( 2, 0, 0xFFFFFFFF, 0x92, 0xCF );  // Kernel mode data segment
	gdt_set_gate( 3, 0, 0xFFFFFFFF, 0xFA, 0xCF );  // User mode code segment
	gdt_set_gate( 4, 0, 0xFFFFFFFF, 0xF2, 0xCF );  // User mode data segment

	gdt_flush( ( u32int ) &gdt_ptr );
}

// Set the value of a GDT entry
static void gdt_set_gate ( s32int num, u32int base, u32int limit, u8int access, u8int gran )
{
	gdt_entries[ num ].base_low    = base & 0xFFFF;
	gdt_entries[ num ].base_middle = ( base >> 16 ) & 0xFF;
	gdt_entries[ num ].base_high   = ( base >> 24 ) & 0xFF;

	gdt_entries[ num ].limit_low   = limit & 0xFFFF;
	gdt_entries[ num ].granularity = ( limit >> 16 ) & 0x0F;

	gdt_entries[ num ].granularity |= gran & 0xF0;
	gdt_entries[ num ].access      = access;
}

//
static void init_idt ()
{
	idt_ptr.limit = ( sizeof( idt_entry_t ) * 256 ) - 1;
	idt_ptr.base  = ( u32int ) &idt_entries;

	// zero all the interrupt service routines
	memset( ( u8int * ) &idt_entries, 0, sizeof( idt_entry_t ) * 256 );

	// Remap the irq table
	remap_pic();

	// ISRs (raised by CPU)
	idt_set_gate(  0, ( u32int )  isr0, 0x08, 0x8E );
	idt_set_gate(  1, ( u32int )  isr1, 0x08, 0x8E );
	idt_set_gate(  2, ( u32int )  isr2, 0x08, 0x8E );
	idt_set_gate(  3, ( u32int )  isr3, 0x08, 0x8E );
	idt_set_gate(  4, ( u32int )  isr4, 0x08, 0x8E );
	idt_set_gate(  5, ( u32int )  isr5, 0x08, 0x8E );
	idt_set_gate(  6, ( u32int )  isr6, 0x08, 0x8E );
	idt_set_gate(  7, ( u32int )  isr7, 0x08, 0x8E );
	idt_set_gate(  8, ( u32int )  isr8, 0x08, 0x8E );
	idt_set_gate(  9, ( u32int )  isr9, 0x08, 0x8E );
	idt_set_gate( 10, ( u32int ) isr10, 0x08, 0x8E );
	idt_set_gate( 11, ( u32int ) isr11, 0x08, 0x8E );
	idt_set_gate( 12, ( u32int ) isr12, 0x08, 0x8E );
	idt_set_gate( 13, ( u32int ) isr13, 0x08, 0x8E );
	idt_set_gate( 14, ( u32int ) isr14, 0x08, 0x8E );
	idt_set_gate( 15, ( u32int ) isr15, 0x08, 0x8E );
	idt_set_gate( 16, ( u32int ) isr16, 0x08, 0x8E );
	idt_set_gate( 17, ( u32int ) isr17, 0x08, 0x8E );
	idt_set_gate( 18, ( u32int ) isr18, 0x08, 0x8E );
	idt_set_gate( 19, ( u32int ) isr19, 0x08, 0x8E );
	idt_set_gate( 20, ( u32int ) isr20, 0x08, 0x8E );
	idt_set_gate( 21, ( u32int ) isr21, 0x08, 0x8E );
	idt_set_gate( 22, ( u32int ) isr22, 0x08, 0x8E );
	idt_set_gate( 23, ( u32int ) isr23, 0x08, 0x8E );
	idt_set_gate( 24, ( u32int ) isr24, 0x08, 0x8E );
	idt_set_gate( 25, ( u32int ) isr25, 0x08, 0x8E );
	idt_set_gate( 26, ( u32int ) isr26, 0x08, 0x8E );
	idt_set_gate( 27, ( u32int ) isr27, 0x08, 0x8E );
	idt_set_gate( 28, ( u32int ) isr28, 0x08, 0x8E );
	idt_set_gate( 29, ( u32int ) isr29, 0x08, 0x8E );
	idt_set_gate( 30, ( u32int ) isr30, 0x08, 0x8E );
	idt_set_gate( 31, ( u32int ) isr31, 0x08, 0x8E );

	// IRQs (raised by external hardware)
	idt_set_gate( 32, ( u32int )  irq0, 0x08, 0x8E );
	idt_set_gate( 33, ( u32int )  irq1, 0x08, 0x8E );
	idt_set_gate( 34, ( u32int )  irq2, 0x08, 0x8E );
	idt_set_gate( 35, ( u32int )  irq3, 0x08, 0x8E );
	idt_set_gate( 36, ( u32int )  irq4, 0x08, 0x8E );
	idt_set_gate( 37, ( u32int )  irq5, 0x08, 0x8E );
	idt_set_gate( 38, ( u32int )  irq6, 0x08, 0x8E );
	idt_set_gate( 39, ( u32int )  irq7, 0x08, 0x8E );
	idt_set_gate( 40, ( u32int )  irq8, 0x08, 0x8E );
	idt_set_gate( 41, ( u32int )  irq9, 0x08, 0x8E );
	idt_set_gate( 42, ( u32int ) irq10, 0x08, 0x8E );
	idt_set_gate( 43, ( u32int ) irq11, 0x08, 0x8E );
	idt_set_gate( 44, ( u32int ) irq12, 0x08, 0x8E );
	idt_set_gate( 45, ( u32int ) irq13, 0x08, 0x8E );
	idt_set_gate( 46, ( u32int ) irq14, 0x08, 0x8E );
	idt_set_gate( 47, ( u32int ) irq15, 0x08, 0x8E );

	idt_flush( ( u32int ) &idt_ptr );
	asm volatile("sti");
}

// Set the value of an IDT entry
static void idt_set_gate ( u8int num, u32int base, u16int sel, u8int flags )
{
	idt_entries[ num ].base_low  = base & 0xFFFF;
	idt_entries[ num ].base_high = ( base >> 16 ) & 0xFFFF;

	idt_entries[ num ].sel       = sel;
	idt_entries[ num ].always0   = 0;

	idt_entries[ num ].flags     = flags;
	// idt_entries[ num ].flags     = flags | 0x60;  // Something about user-mode, sets privilege level to 3
}


