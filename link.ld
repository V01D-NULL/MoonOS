/*
  Linker script for the kernel
  Ensures everything goes in the correct place

  Tells LD how to setup our kernel image. Tells it,
    . start location should by the symbol 'start'
    . text section (where all our code goes) should be first and start at 0x100000
    . data (initialised static data) and bss (uninitialised static data) should be next and each page-aligned
    . rodata (read only initialized data) bundled with data
*/
OUTPUT_ARCH(i386)
ENTRY( start )
SECTIONS
{	
	. = 0x100000;
	kernel_start = .;
	.text :
	{
		/* Multiboot (just places the multiboot structure in the first 8kb of the elf file */
		. = ALIGN(8);
		KEEP(*(.multiboot))

		/* Kernel code */
		code   = .;
		_code  = .;
		__code = .;

		*( .text )
		. = ALIGN( 4096 );
	}

	.data :
	{
		data   = .;
		_data  = .;
		__data = .;

		*( .data )
		*( .rodata )
		. = ALIGN( 4096 );	
	}

	.bss :
	{
		bss   = .;
		_bss  = .;
		__bss = .;

		*( .bss )
		. = ALIGN( 4096 );
	}

	. = ALIGN(4096); /* page align */
	kernel_end = .;
}