#include "serial.h"

static int serial_received() {
    return (inb(COM1+5)&1);
}

char serial_read() {
    while(serial_received() == 0);

    return inb(COM1);
}

static int is_transit_empty() {
    return (inb(COM1+5)&0x20);
}

void serial_write(char chr) {
    while (is_transit_empty() == 0);

    outb(COM1, chr);
}

void serial_write_str(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        serial_write(str[i]);
    }
}

void serial_write_dec(uint64_t n)
{
    char  c [64];
	char c2 [64];
	int64_t i;
	int64_t j;

	if (n == 0)
	{
		serial_write('0');
		return;
	}

	int64_t acc = n;
	// signed int acc = n;

	i = 0;
	while (acc > 0)
	{
		c[i] = acc % 10 + '0';  //

		acc /= 10;

		i += 1;
	}
	c[i] = 0;  // ?

	//
	c2[ i ] = 0;  // ?
	i -= 1;

	// Reverse order so that MSD stored at index 0 ?
	j = 0;
	while ( i >= 0 )
	{
		c2[i] = c[j];

		i -= 1;
		j += 1;
	}

	serial_write_str(c2);
}

void serial_write_hex(uint64_t n)
{
    signed int tmp;
	int i;

    serial_write_str("0x");

	char noZeroes = 1;

	for ( i = 28; i > 0; i -= 4 )
	{
		tmp = ( n >> i ) & 0xF;

		if ( tmp == 0 && noZeroes != 0 )
		{
			continue;
		}

		if ( tmp >= 0xA )
		{
			noZeroes = 0;
            serial_write(tmp - 0xA + 'a'); //a..f
		}
		else
		{
			noZeroes = 0;
            serial_write(tmp + '0'); // 0..9
		}
	}

	// last nibble
	tmp = n & 0xF;

	if ( tmp >= 0xA )
	{
		serial_write(tmp - 0xA + 'a'); //a..f
	}
	else
	{
		serial_write(tmp + '0'); // 0..9
	}
}

//Writes a bash color code which will change the color of the output
void serial_set_color(const char* color_code)
{
    if (color_code[0] != '\e')
        return;

    serial_write_str(color_code);
}