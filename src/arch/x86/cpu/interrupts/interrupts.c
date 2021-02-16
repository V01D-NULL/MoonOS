#include "interrupts.h"
#include "../../../../drivers/screen/monitor.h"
#include "../../../../panic.h"

isr_t interrupt_handlers [ 256 ];

//Straight up copy and paste the text from the intel programming manual :D
char* reserved_exceptions[] = {
   "Type: (#DE) Division Exception",
   "Type: (#DB) Debug Exception",
   "Type: (NONE) NMI interrupt (Non Maskable Interrupt)",
   "Type: (#BP) Breakpoint Exception",
   "Type: (#OF) Into detected Overflow Exception",
   "Type: (#BR) BOUND Range Exceeded",
   "Type: (#UD) Invalid Opcode (Undefined Opcode)",
   "Type: (#NM) Device Not Available (No Math Coprocessor)",
   "Type: (#DF) Double Fault",
   "Type: (NONE) Coprocessor Segment Overrun (reserved)",
   "Type: (#TS) Invalid TSS",
   "Type: (#NP) Segment Not Present",
   "Type: (#SS) Stack-Segment Fault",
   "Type: (#GP) General Protection Fault",
   "Type: (#PF) Page Fault",
   "Type: (NONE) Unknown interrupt",
   "Type: (#CF) Coprocessor fault",
   "Type: (#AC) Alignment check",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved",
   "Type: (NONE) Reserved"
};

// This gets called by the ASM interrupt handler stub
void isr_handler( registers_t regs )
{
	set_color(VGA_BLACK, VGA_RED);
	monitor_write("----------------------------------\n", false, false);
	set_color(VGA_BLACK, VGA_WHITE);
	monitor_write("Recieved interrupt: ", false, false);
	monitor_write_dec( regs.int_no );
	monitor_writeln();
	monitor_write(reserved_exceptions[regs.int_no], false, false);
	monitor_write("\nError code: ", false, false);
	monitor_write_dec(regs.err_code);
	monitor_writeln();
	set_color(VGA_BLACK, VGA_RED);
	monitor_write("----------------------------------\n", false, false);
	set_color(VGA_BLACK, VGA_WHITE);

	serial_set_color(BASH_RED);
	debug("----------------------------------\n");
	serial_set_color(BASH_WHITE);
	debug("Recieved interrupt: %d\n%s\nError code present? ~ %d\n", regs.int_no, reserved_exceptions[regs.int_no], regs.err_code);
	
	dump_regs_interrupt(regs);
	serial_set_color(BASH_RED);
	debug("----------------------------------\n");
	serial_set_color(BASH_WHITE);

	// Execute handler associated with interrupt
	if ( interrupt_handlers[ regs.int_no ] != 0 )
	{
		isr_t handler = interrupt_handlers[ regs.int_no ];

		handler( regs );
	}
	
}

void register_interrupt_handler ( u8int n, isr_t handler )
{
	interrupt_handlers[ n ] = handler;
}

void unregister_interrupt_hanlder(u8int n) {
	interrupt_handlers[ n ] = 0;
}

// This gets called from our ASM interrupt handler stub
void irq_handler( registers_t regs )
{
	// Send an end of interrupt signal to the PICs (programmable interrupt controllers)

	// if this interrupt involved PIC2 (IRQ > 7)
	if ( regs.int_no >= 40 )
	{
		// send reset command to PIC2 (signal end of interrupt)
		slave_eoi();
	}
	
	// send reset command to PIC1 (signal end of interrupt)
	master_eoi();

	// Execute handler associated with interrupt
	if ( interrupt_handlers[ regs.int_no ] != 0 )
	{
		isr_t handler = interrupt_handlers[ regs.int_no ];

		handler( regs );
	}
}
