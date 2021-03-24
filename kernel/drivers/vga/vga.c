#include "vga.h"


void vga_init(int fg, int bg) {
	vga.col = 80;
	vga.row = 25;
	vga.cursor_x = 0;
	vga.cursor_y = 0;
	vga.vram = (uint16_t*)0xB8000;
	vga.attr = ((bg << 4) | (fg & 0x0F)) << 8;
	vga.text_scroll_speed = 4;
	vga_hist.cell_color_bg = bg;
	vga_hist.cell_color_fg = fg;
}

uint8_t get_x () { return vga.cursor_x;}
uint8_t get_y () { return vga.cursor_y;}

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
	int i;
	int lastRow = vga.row - 1;

	uint16_t spaceChar = vga.attr | 0x20;  // space character

	// Move the current text chunk that makes up the screen back in the buffer by a line
	for ( i = 0; i < lastRow * vga.col; i += 1 )
	{
		vga.vram[ i ] = vga.vram[ i + vga.col ];
	}

	// The last line should now be blank. Do this by writing 80 spaces to it
	for ( i = lastRow * vga.col; i < vga.row * vga.col; i += 1 )
	{
		vga.vram[ i ] = spaceChar;
	}

	// The cursor should now be on the last line
	vga.cursor_y = lastRow;
}

// Write a single character out to the screen
void vga_putc(char c)
{
	uint16_t *location;

	// Handle a backspace
	if ( c == 0x08 && vga.cursor_x )
	{
		vga.cursor_x -= 1; //Reset cursor position

		//Put an empty character inplace of the character there was before it.
		location = vga.vram + ( vga.cursor_y * vga.col + vga.cursor_x );
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

	// Handle newline by moving cursor back to left and incrementing the row
	else if ( c == '\n' )
	{
		vga.cursor_x = 0;
		vga.cursor_y++;
	}

	// Handle any other printable character
	else if ( c >= ' ' && c < 127 )
	{
		location = vga.vram + ( vga.cursor_y * vga.col + vga.cursor_x );

		*location = vga.attr | c;

		vga.cursor_x++;
	}

	// Check if we need to inser a new line because we've reached the end of the screen
	if ( vga.cursor_x >= vga.col )
	{
		vga.cursor_x = 0;
		vga.cursor_y++;
	}

	// Scroll the screen if needed
	if ( vga.cursor_y >= vga.row )
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
	for ( i = 0; i < vga.row * vga.col; i += 1 )
	{
		vga.vram[ i ] = spaceChar;
	}

	// Move the hardware cursor back to the start
	vga.cursor_x = 0;
	vga.cursor_y = 0;
	vga_move_cursor();
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

int kprintf(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buff, (size_t) -1, fmt, arg);
	va_end(arg);

	vga_puts((char*)&buff, false, false);

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