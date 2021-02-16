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

void serial_write_dec(u32int n)
{
    switch (n) {
        case 0:
            serial_write('0');
            break;
        case 1:
            serial_write('1');
            break;
        case 2:
            serial_write('2');
            break;
        case 3:
            serial_write('3');
            break;
        case 4:
            serial_write('4');
            break;
        case 5:
            serial_write('5');
            break;
        case 6:
            serial_write('6');
            break;
        case 7:
            serial_write('7');
            break;
        case 8:
            serial_write('8');
            break;
        case 9:
            serial_write('9');
            break;
        
        default:
            break;
    }
}

void serial_write_hex(u32int n)
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