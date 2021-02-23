#include "panic.h"
#include "arch/x86/cpu/interrupts/interrupts.h"
#include "common.h"
#include "drivers/io/serial.h"
#include "drivers/screen/monitor.h"


//Specific to interrupts
void dump_regs_interrupt(registers_t r)
{
    serial_set_color(BASH_YELLOW);
    debug("\n*******************************\n");
    serial_set_color(BASH_WHITE);
    debug("BEGIN OF REGISTER DUMP\n");
    
    //Split up the interrupt register dump into multiple debug statements for better reading
    debug("CS -> %x\t\tDS -> %x\nEDI -> %x\t\tESI -> %x\n", r.cs, r.ds, r.edi, r.esi);
    debug("EAX -> %x\tEBX -> %x\nECX -> %x\t\tEDX -> %x\n", r.eax, r.ebx, r.ecx, r.edx);
    debug("EIP -> %x\t\tEFLAGS -> %x\nUSER_ESP -> %x\t\tSS -> %x\n", r.eip, r.eflags, r.useresp, r.ss);
    debug("EBP -> %x\t\tESP -> %x \n");

    debug("END OF REGISTER DUMP\n");
    serial_set_color(BASH_YELLOW);
    debug("*******************************\n");
    serial_set_color(BASH_WHITE);
}

//General register dump
void dump_regs(void)
{
    
    register int eax asm ("eax");
    register int ebx asm ("ebx");
    register int ecx asm ("ecx");
    register int edx asm ("edx");
    register int esp asm ("esp");
    register int ebp asm ("ebp");
    
    asm volatile ("pusha"); //Prevent these functions from overwriting the registers I just saved
    debug("\n::: GENERAL REGISTER DUMP :::\n*******************************\n");
    debug("BEGIN OF REGISTER DUMP\n");
    debug("EAX -> %x\t\tEBX -> %x\nECX -> %x\t\tEDX -> %x\nEBP -> %x\t\tESP -> %x\n", eax, ebx, ecx, edx, ebp, esp);
    debug("END OF REGISTER DUMP\n");
    // debug("\nVirtual memory: %X\n", get_memory_offset());
    debug("*******************************\n", false, false);
    asm volatile ("popa");
}

// Prints message and enters infinite loop (stopping program execution)
extern void panic_assert ( const char *file, u32int line, const char *desc )
{
	// An assertion failed, and we have to panic
	asm volatile( "cli" );  // disable interrupts

    set_color(VGA_BLACK, VGA_RED);
    monitor_write("\nFATAL ERROR: ", false, false);
    set_color(VGA_BLACK, VGA_WHITE);
    monitor_put('(');
	monitor_write( ( char * ) desc,0,0 		);
	monitor_write( ") at ", false, 0   		);
	monitor_write( ( char * ) file, false,0 );
	monitor_write( ":", false ,false   		);
	monitor_write_dec( line            		);
	monitor_write( "\n", false, false  		);

    dump_regs();

	// Halt by going into an infinite loop
	for( ;; );
}

extern int assert ( const char *file, u32int line, const char *desc )
{
    monitor_write("ASSERTION_FAILED: (", false, false   );
	monitor_write( ( char * ) desc, false, false 		);
	monitor_write( ") at ", false, false                );
	monitor_write( ( char * ) file, false, false        );
	monitor_write( ":", false ,false   		            );
	monitor_write_dec( line            		            );
	monitor_write( "\n", false, false  		            );
    return -1;
}