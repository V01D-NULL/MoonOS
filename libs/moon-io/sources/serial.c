#include "moon-io/serial.h"
#include "moon-io/port-io.h"
#include <base/fmt.h>
#include <base/string.h>

#define PRINT_LINE_NUM(file, line_buff) \
	{                                   \
		serial_set_color(BASH_GREEN);   \
		serial_write_str(file);         \
		serial_write_str(":");          \
		serial_write_str(line_buff);    \
		serial_set_color(BASH_WHITE);   \
		serial_write_str(" ");          \
	}

static int serial_received(void)
{
	return (inb(COM1 + 5) & 1);
}

char serial_read(void)
{
	while (serial_received() == 0)
		;

	return inb(COM1);
}

static int is_transit_empty(void)
{
	return (inb(COM1 + 5) & 0x20);
}

void serial_write(char chr)
{
	while (is_transit_empty() == 0)
		;

	outb(COM1, chr);
}

void serial_write_str(string_view str)
{
	for (uint64_t i = 0; i < strlen(str); i++)
	{
		serial_write(str[i]);
	}
}

// Writes a bash color code which will change the color of the output
void serial_set_color(string_view color_code)
{
	serial_write_str(color_code);
}

char serial_buff[512];
void impl_verbose_debug(string file, int line, string fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((string)&serial_buff, (size_t)-1, fmt, arg);
	va_end(arg);

	char line_buff[7];
	sprintf(line_buff, "%d", line);
	PRINT_LINE_NUM(file, line_buff);

	serial_write_str((string)&serial_buff);
}

char serial_q_buff[512];
void impl_quiet_debug(string fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	vsnprintf((string)&serial_q_buff, (size_t)-1, fmt, arg);
	va_end(arg);

	serial_write_str((string)&serial_q_buff);
}