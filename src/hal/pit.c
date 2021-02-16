// Initialises the PIT (programmable interval timer), and handles clock updates

#include "pit.h"
#include "../arch/x86/cpu/interrupts/interrupts.h"
#include "../drivers/screen/monitor.h"
#include "../sys/timer.h"

/*
	command 0x43, data 0x40
*/

void init_timer ( u32int frequency )  // frequency in Hz
{
	// The value we send to the PIT is the value to divide its input clock
	// (1.193180 MHz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	u32int divisor = 1193180 / frequency;

	// Send command byte
	/*
		00110110

		7..6  |  00   channel: 0
		5..4  |  11   access mode: lo/hi byte
		3..1  |  011  operating mode: square wave generator?, IRQ0 on rising edge
		   0  |  0    bcd/binary mode: 16-bit binary
	*/
	outb( 0x43, 0x36 );

	// Prepare divisor for sending
	u8int lo = ( u8int ) ( divisor & 0xFF );
	u8int hi = ( u8int ) ( divisor >> 8 ) & 0xFF;

	// Send the frequency divisor
	outb( 0x40, lo );
	outb( 0x40, hi );
}

static void play_sound(u32int freq)
{
	u32int divisor = 1193180 / freq;
	u8int  tmp;

	//Set the frequency
	outb(0x43, 0xb6);
 	outb(0x42, (u8int) (divisor) );
 	outb(0x42, (u8int) (divisor >> 8));

	//Play the sound
	tmp = inb(0x61);
	if (tmp != (tmp | 3)) 																											
	{
		outb(0x61, tmp | 3);
	}

}

static void stop_sound() {
	u8int tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}

void sleep(u32int ms);
void beep(u32int freq) {
	play_sound(freq);
	sleep(1);
	stop_sound();
}

