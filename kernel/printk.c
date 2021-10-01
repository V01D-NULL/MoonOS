#include "printk.h"
#include <drivers/vbe/vbe.h>
#include <3rdParty/ssfn.h>

extern gfx_header_t gfx_h;

long char_cnt = 0;
void putc(uint32_t unicode, int x, int y)
{
    if (x != -1 && y != -1)
    {
        ssfn_dst.x = x;
        ssfn_dst.y = y;
    }
    
    ssfn_putc(unicode);
    char_cnt++;
}

void puts(const char *str)
{
    for (int i = 0, n = strlen(str); i < n; i++)
    {
        //Handle text wrapping
        if (ssfn_dst.x >= ssfn_dst.w)
        {
            ssfn_putc('\n');
        }

        if (str[i] == '\t')
            for(int j = 0; j < 4; j++)
                ssfn_putc(' ');
        else
            ssfn_putc(str[i]);
        char_cnt++;
    }
    if (ssfn_dst.y >= gfx_h.fb_height)
    {
        gfx_scroll(1);
    }
}

char buffer[512];

//Use this when waiting for a job to complete (it's poorly coded but, meh. Writing to video memory is fast anyway it's fine)
//TODO: Create a queue.c file where a function pointer is defined.
// All queued functions will be tracked with a linked list.
// Insert the function pointer in printk_wait and once the function is done modify the hourglass to an OK or ERR (depending on what the function returns)
void printk_wait(bool (*callback)(), char *fmt,  ...)
{
    va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buffer, (size_t) -1, fmt, arg);
	va_end(arg);

    putc('[', -1, -1);
    gfx_set_colors(0xffffff, 0x0);
    putc(OPERATION_BUSY, -1, -1);
    gfx_restore_colors();
    puts("] ");
    
    puts((const char*)&buffer);
    ssfn_dst.y = ssfn_dst.y - GLYPH_HEIGHT; //Don't increment y

    //Call function pointer
    bool ret = (*callback) ();

    //Delete line with the hourglass
    gfx_clear_x(ssfn_dst.y, 0x0);
    
    //Reset x position
    ssfn_dst.x = 0;

    //Check return type (true = ok, false = err)
    if (ret == true)
    {
        putc('[', -1, -1);
        gfx_set_colors(0x4CC417, 0x0);
        putc(OPERATION_OK, -1, -1);
        gfx_restore_colors();
        puts("] ");
    }
    else
    {
        putc('[', -1, -1);
        gfx_set_colors(0xF70D1A, 0x0);
        putc(OPERATION_FAIL, -1, -1);
        gfx_restore_colors();
        puts("] ");   
    }
    puts((const char*)&buffer);
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buffer, (size_t) -1, fmt, arg);
	va_end(arg);

    if (strcmp(status, "OK") == 0)
    {
        putc('[', -1, -1);
        gfx_set_colors(0x4CC417, 0x0);
        putc(OPERATION_OK, -1, -1);
        gfx_restore_colors();
        puts("] ");
    }
    else if (strcmp(status, "ERR") == 0)
    {
        putc('[', -1, -1);
        gfx_set_colors(0xF70D1A, 0x0);
        putc(OPERATION_FAIL, -1, -1);
        gfx_restore_colors();
        puts("] ");
        
    }
    else
    {
        puts("[ ");
        puts(status);
        puts(" ] ");
    }
    puts((const char*)&buffer);
    #ifdef USE_VGA
    vga_puts((char*)&buffer, false, false);
    #endif
    delay(20);
}