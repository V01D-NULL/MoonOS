/*
  Linker script for the kernel
  Ensures everything goes in the correct place

  Tells LD how to setup our kernel image. Tells it,
    . start location should by the symbol 'start'
    . text section (where all our code goes) should be first and start at 0x100000
    . data (initialised static data) and bss (uninitialised static data) should be next and each page-aligned
    . rodata (read only initialized data) bundled with data
*/
/*OUTPUT_ARCH(elf64-x86-64)*/
ENTRY( start )
SECTIONS
{
	
	.text 0x100000 :
	{
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

	end   = .;
	_end  = .;
	__end = .;
}
