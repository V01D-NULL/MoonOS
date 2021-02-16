#include "cursor.h"

// Updates the hardware cursor
void move_cursor ()
{
	u16int cursorLocation = get_y() * NCOLS + get_x();

	// For some reason VGA accepts 16-bit location as two bytes
	outb( 0x3D4, 14 );                   // Tell VGA we are setting the high cursor byte
	outb( 0x3D5, cursorLocation >> 8 );  // Send the high cursor byte
	outb( 0x3D4, 15 );                   // Tell VGA we are setting the low byte
	outb( 0x3D5, cursorLocation );       // Send it
}

void kill_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void enable_cursor(u8int cursor_start, u8int cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}