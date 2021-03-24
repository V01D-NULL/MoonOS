#include "monitor.h"

/*
	Something about booting by GRUB in text mode. Thus framebuffer controls characters (not pixels).
	Accessible at address 0xB8000. Part of VGA controller's memory that has been memory-mapped via
	hardware into our linear address space.

	The frame buffer is an array of 16-bit words, with each representing the display of one character.
	It represents an area 80 wide, 25 high.
		idx = ( y * 80 + x ) * 2   // x2 because 16-bit and 32-bit...?
	
	8 bits are used to represent a character, the other 8 are used to to designate the foreground and
	background colors (4 bits each).
		FEDC | BA98  | 76543210
		----- ------- ------------
		bgCol| fgCol | ASCII code

	Possible colors
	 0 - black
	 1 - blue
	 2 - green
	 3 - cyan
	 4 - red
	 5 - magenta
	 6 - brown
	 7 - light grey
	 8 - dark grey
	 9 - light blue
	10 - light green
	11 - light cyan
	12 - light red
	13 - light magenta
	14 - light brown
	15 - white

	VGA's control register 0x3D4 and data register 0x3D5 are used to update the cursor position
*/

//

// cursor position
uint8_t cursorX = 0;
uint8_t cursorY = 0;

uint8_t get_x () { return cursorX;}
uint8_t get_y () { return cursorY;}

/* Default colors (Black on white)*/
// const int BGCOLOR = 0;
// const int FGCOLOR = 15;

// #define NROWS_HISTORY NROWS * 5

// VGA framebuffer starts at 0xB8000
uint16_t *videoMemory = ( uint16_t * ) 0xB8000;

//
// u16int *textBuffer [ NCOLS * NROWS * 5 ];  // history

static uint32_t text_scroll_speed = 4;

//Default color. Black bg, white fg
uint16_t m_attribute = ( ( 0 << 4 ) | ( 15 & 0x0F ) ) << 8;

//Feel free to change the color here.
void set_color(const int BGCOLOR, const int FGCOLOR) {
	m_attribute = ( ( BGCOLOR << 4 ) | ( FGCOLOR & 0x0F ) ) << 8;
}

void set_scroll_speed(uint32_t speed) {
	debug("Set scroll speed from %d to %d\n", text_scroll_speed, speed);
	text_scroll_speed = speed;
}

// Scrolls the text on the screen up by one line
void monitor_scrollUp ()
{
	int i;
	int lastRow = NROWS - 1;

	uint16_t spaceChar = m_attribute | 0x20;  // space character

	// Move the current text chunk that makes up the screen back in the buffer by a line
	for ( i = 0; i < lastRow * NCOLS; i += 1 )
	{
		videoMemory[ i ] = videoMemory[ i + NCOLS ];
	}

	// The last line should now be blank. Do this by writing 80 spaces to it
	for ( i = lastRow * NCOLS; i < NROWS * NCOLS; i += 1 )
	{
		videoMemory[ i ] = spaceChar;
	}

	// The cursor should now be on the last line
	cursorY = lastRow;
}

// Scrolls the text on the screen down by one line
void monitor_scrollDown ()
{
	// TODO
}

// Write a single character out to the screen
void monitor_put ( char c )
{
	uint16_t *location;

	// Handle a backspace
	if ( c == 0x08 && cursorX )
	{
		cursorX -= 1; //Reset cursor position

		//Put an empty character inplace of the character there was before it.
		location = videoMemory + ( cursorY * NCOLS + cursorX );
		*location = m_attribute | ' ';
	}

	// Handle a tab by increasing the cursor's x-pos, but only to a point where it's divisible by 8
	else if ( c == 0x09 )
	{
		cursorX = ( cursorX + 8 ) & ~ ( 8 - 1 );
	}

	// Handle a carriage return
	else if ( c == '\r' )
	{
		cursorX = 0;
	}

	// Handle newline by moving cursor back to left and incrementing the row
	else if ( c == '\n' )
	{
		cursorX = 0;
		cursorY++;
	}

	// Handle any other printable character
	else if ( c >= ' ' && c < 127 )
	{
		location = videoMemory + ( cursorY * NCOLS + cursorX );

		*location = m_attribute | c;

		cursorX++;
	}

	// Check if we need to inser a new line because we've reached the end of the screen
	if ( cursorX >= NCOLS )
	{
		cursorX = 0;
		cursorY++;
	}

	// Scroll the screen if needed
	if ( cursorY >= NROWS )
	{
		monitor_scrollUp();
	}

	// Move the hardware cursor
	move_cursor();
}

// Clear the screen to all black by writting spaces to the framebuffer
void monitor_clear ()
{
	int i;

	uint16_t spaceChar = m_attribute | 0x20;  // space character

	// Fill buffer with spaces
	for ( i = 0; i < NROWS * NCOLS; i += 1 )
	{
		videoMemory[ i ] = spaceChar;
	}

	// Move the hardware cursor back to the start
	cursorX = 0;
	cursorY = 0;
	move_cursor();
}

void op_ok()
{
	monitor_put('[');
	set_color(VGA_BLACK, VGA_GREEN);
	monitor_put(' ');
	monitor_put('O');
	monitor_put('K');
	set_color(0, 15);
	monitor_put(' ');
	monitor_put(']');
	monitor_put(' ');
	set_color(VGA_BLACK, VGA_WHITE);
}

void op_task()
{
	char task[4] = "Task";
	monitor_put('[');
	set_color(VGA_BLACK, VGA_LIGHT_GREY);
	
	for(int i = 0; i < 4; i++)
		monitor_put(task[i]);
	set_color(VGA_BLACK, VGA_WHITE);
	monitor_put(']');
	monitor_put(' ');
}

// Output a null-terminated ASCII string to the monitor
void monitor_write (char *c, bool is_task, bool operation_ok)
{
	if (is_task && operation_ok) { return; } //Must be either a task, an operation_ok, or none. Not both. It will flood the terminal
	delay(text_scroll_speed);
	if (is_task) {
		op_task();
	}
	else if (operation_ok)
	{
		op_ok();
	}
	
	for (int i = 0; i < strlen(c); i++)
	{
		monitor_put(c[i]);
	}
}

// Write a newline character
void monitor_writeln ()
{
	delay(1);
	monitor_put( '\n' );
}

int debug(char* fmt, ...)
{
	if (!fmt)
		return -1;

	va_list arg;
	
	va_start(arg, fmt);

	for (int i = 0; i < strlen(fmt); i++)
	{
		switch (fmt[i])
		{
			case '%':
			{
				switch (fmt[i+1])
				{
					case 'c':
					{
						char character = va_arg(arg, int);
						serial_write(character);
						i+=2;
						break;
					}

					case 's':
					{
						const char* string = va_arg(arg, char*);
						serial_write_str(string);
						i+=2;
						break;
					}

					case 'i':
					case 'd':
					{
						int val = va_arg(arg, int);
						char result[256];
						serial_write_str(itoa(val, result, BASE_10));
						i += 2;
						break;
					}

					case 'x':
					case 'X':
					{
						int hex = va_arg(arg, int);
						char result[256];
						serial_write_str(itoa(hex, result, BASE_16));
						i += 2;
						break;
					}

					case 'b':
					{
						int bin = va_arg(arg, int);
						char result[256];
						serial_write_str(itoa(bin, result, BASE_2));
						i+=2;
						break;
					}

					default:
						break;
				}
			}
			default:
				serial_write(fmt[i]);
		}
	}

	va_end(arg);
	return 0; //All went well
}

int kprintf(const char* fmt, ...)
{
	if (!fmt)
		return -1;

	va_list arg;
	
	va_start(arg, fmt);

	for (int i = 0; i < strlen(fmt); i++)
	{
		switch (fmt[i])
		{
			case '%':
			{
				switch (fmt[i+1])
				{
					case 'c':
					{
						char character = va_arg(arg, int);
						monitor_put(character);
						i+=2;
						break;
					}

					case 's':
					{
						char* string = va_arg(arg, char*);
						monitor_write(string, false, false);
						i+=2;
						break;
					}

					case 'i':
					case 'd':
					{
						int val = va_arg(arg, int);
						char result[256];
						monitor_write(itoa(val, result, BASE_10), false, false);
						i += 2;
						break;
					}

					case 'x':
					case 'X':
					{
						uint64_t hex = va_arg(arg, int);
						char result[256];
						monitor_write(itoa(hex, result, BASE_16), false, false);
						i += 2;
						break;
					}

					case 'b':
					{
						int bin = va_arg(arg, int);
						char result[256];
						monitor_write(itoa(bin, result, BASE_2), false, false);
						i+=2;
						break;
					}

					default:
						break;
				}
			}
			default:
				monitor_put(fmt[i]);
		}
	}

	va_end(arg);
	return 0; //All went well
}

//Write something to the X line and clear it again.
void write_x(const char* str) 
{
	for (int i = 0; i < strlen(str); i++)
		monitor_put(str[i]);
	cursorX = 0;
}

void write_x_dec(uint64_t n)
{
	//monitor_write_dec(n);
	cursorX = 0;
}

void write_x_hex(uint64_t n)
{
	//monitor_write_hex(n);
	cursorX = 0;
}

//Resets the Cursor X after printing. This way things like timers and dates can be shown.
int kprintf_x(const char* fmt, ...)
{
	if (!fmt)
		return -1;

	va_list arg;
	
	va_start(arg, fmt);

	for (int i = 0; i < strlen(fmt); i++)
	{
		switch (fmt[i])
		{
			case '%':
			{
				switch (fmt[i+1])
				{
					case 'c':
					{
						char character = va_arg(arg, int);
						monitor_put(character);
						i+=2;
						break;
					}

					case 's':
					{
						char* string = va_arg(arg, char*);
						monitor_write(string, false, false);
						i+=2;
						break;
					}

					case 'i':
					case 'd':
					{
						uint64_t val = va_arg(arg, int);
						char result[256];
						monitor_write(itoa(val, result, BASE_10), false, false);
						i += 2;
						break;
					}

					case 'x':
					case 'X':
					{
						uint64_t hex = va_arg(arg, int);
						monitor_write(itoh(hex), false, false);
						i += 2;
						break;
					}

					case 'b':
					{
						int bin = va_arg(arg, int);
						//monitor_write(itob(bin), false, false);
						i+=2;
						break;
					}

					default:
						break;
				}
			}
			default:
				monitor_put(fmt[i]);
		}
	}

	va_end(arg);
	cursorX = 0;
	return 0; //All went well
}