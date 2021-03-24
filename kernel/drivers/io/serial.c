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

//Writes a bash color code which will change the color of the output
void serial_set_color(const char* color_code)
{
    if (color_code[0] != '\e')
        return;

    serial_write_str(color_code);
}