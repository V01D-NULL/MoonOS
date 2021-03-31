#include "vga.h"
#include <stdint.h>

//Init structs
vga_hdr_t vga;
vga_history_t vga_hist;

void vga_init(int fg, int bg) {
	vga.row = 80;
	vga.col = 25;
	vga.cursor_x = 0;
	vga.cursor_y = 0;
	vga.vram = (uint16_t*)0xB8000;
	set_color(bg, fg);
	vga.text_scroll_speed = 10;
}

// #define vga.row_HISTORY vga.row * 5

// u16int *textBuffer [ vga.col * vga.row * 5 ];  // history

void set_color(const int BGCOLOR, const int FGCOLOR) {
	//Save the Fore & background colors
	vga_hist.cell_color_bg = BGCOLOR;
	vga_hist.cell_color_fg = FGCOLOR;

	//Set the colors
	vga.attr = ((BGCOLOR << 4) | (FGCOLOR & 0x0F)) << 8;
}

void set_scroll_speed(uint32_t speed) {
	debug("Set scroll speed from %d to %d\n", vga.text_scroll_speed, speed);
	vga.text_scroll_speed = speed;
}

// Scrolls the text on the screen up by one line
void vga_scroll()
{    
    int i = 0;
    for(i = 0; i < (vga.row*vga.col-80); i++)
    	vga.vram[i] = vga.vram[i+80];
    for(i = 0; i < vga.row; i++)
        vga.vram[(vga.row - 1) * vga.row + i] = (uint16_t) ' ' | ((uint16_t) VGA_BLACK << 8);

}

// Write a single character out to the screen
void vga_putc(char c)
{
	uint16_t *location = 0;

	// Handle a backspace
	if (c == 0x08 && vga.cursor_x != 0)
	{
		vga.cursor_x -= 1; //Reset cursor position

		//Put an empty character inplace of the character there was before it.
		location = vga.vram + ( vga.cursor_y * vga.row + vga.cursor_x );
		*location = vga.attr | ' ';
	}

	// Handle a tab by increasing the cursor's x-pos, but only to a point where it's divisible by 8
	else if ( c == 0x09 )
	{
		vga.cursor_x = ( vga.cursor_x + 8 ) & ~ ( 8 - 1 );
	}

	// Handle a carriage return
	else if ( c == '\r' )
	{
		vga.cursor_x = 0;
	}

	// Handle newline
	else if ( c == '\n' )
	{
		vga.cursor_x = 0;
		vga.cursor_y++;
	}

	// Handle any other printable character
	else if ( c >= ' ' && c < 127 )
	{
		location = vga.vram + ( vga.cursor_y * vga.row + vga.cursor_x );

		*location = vga.attr | c;

		vga.cursor_x++;
	}

	if ( vga.cursor_x >= vga.row )
	{
		vga.cursor_x = 0;
		vga.cursor_y++;
	}

	// Scroll the screen if needed
	if ( vga.cursor_y >= vga.col )
	{
		vga_scroll();
	}

	// Move the hardware cursor
	vga_move_cursor();
}

// Clear the screen to all black by writting spaces to the framebuffer
void vga_clear()
{
	int i;

	uint16_t spaceChar = vga.attr | 0x20;  // space character

	// Fill buffer with spaces
	for (i = 0; i < vga.row * vga.col; i++)
	{
		vga.vram[ i ] = spaceChar;
	}

	// Move the hardware cursor back to the start
	vga.cursor_x = 0;
	vga.cursor_y = 0;
	vga_move_cursor();
	vga.vram = (uint16_t*)0xB8000; //Reset vram framebuffer
}

void op_ok()
{
	int last_fg = vga_hist.cell_color_fg;
	int last_bg = vga_hist.cell_color_bg;
	set_color(VGA_BLACK, VGA_WHITE);
	vga_putc('[');
	set_color(VGA_BLACK, VGA_GREEN);
	vga_putc(' ');
	vga_putc('O');
	vga_putc('K');
	set_color(0, 15); 
	vga_putc(' ');
	vga_putc(']');
	vga_putc(' ');
	set_color(last_bg, last_fg); //Restore last known background and foreground color
}

void op_task()
{
	char task[5] = "Task";
	int last_fg = vga_hist.cell_color_fg;
	int last_bg = vga_hist.cell_color_bg;
	
	set_color(VGA_BLACK, VGA_WHITE);
	vga_putc('[');
	set_color(VGA_BLACK, VGA_LIGHT_GREY);
	
	for(int i = 0; i < 4; i++)
		vga_putc(task[i]);
	
	set_color(VGA_BLACK, VGA_WHITE);
	vga_putc(']');
	vga_putc(' ');
	set_color(last_bg, last_fg); //Restore last known background and foreground color
}

// Output a null-terminated ASCII string to the monitor
void vga_puts(char *c, bool is_task, bool operation_ok)
{
	if (is_task && operation_ok) { return; } //Must be either a task, an operation_ok, or none. Not both. It will flood the terminal
	delay(vga.text_scroll_speed);
	if (is_task) {
		op_task();
	}
	else if (operation_ok)
	{
		op_ok();
	}
	
	for (int i = 0; i < strlen(c); i++)
	{
		vga_putc(c[i]);
	}
}

char buff[512] = {0};
int debug(char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buff, (size_t) -1, fmt, arg);
	va_end(arg);
	
	serial_write_str((char*)&buff);
	
	return 0; //All went well
}

char buff2[512] = {0};
int kprintf(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buff2, (size_t) -1, fmt, arg);
	va_end(arg);

	vga_puts((char*)&buff2, false, false);

	return 0; //All went well
}

//Kernel Printf Pretty (makes use of the vga_puts ok and task)
int kprintf_p(bool is_task, bool operation_ok, const char* fmt,  ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buff, (size_t) -1, fmt, arg);
	va_end(arg);

	vga_puts((char*)&buff, is_task, operation_ok);

	return 0; //All went well
}
