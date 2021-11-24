#include "vga.cursor.h"
#include "vga.h"
#include <amd64/moon.h>

gnu_export vga_hdr_t vga;

// Updates the hardware cursor
void vga_move_cursor(void)
{
	
	uint16_t cursorLocation = vga.cursor_y * vga.row + vga.cursor_x;
	
	outb(0x3D4, 14);                   // Tell VGA we are setting the high cursor byte
	outb(0x3D5, cursorLocation >> 8);  // Send the high cursor byte
	outb(0x3D4, 15);                   // Tell VGA we are setting the low byte
	outb(0x3D5, cursorLocation);       // Send it
}

void vga_kill_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}