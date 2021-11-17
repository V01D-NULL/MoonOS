#include "tty.utils.h"
#include <libgraphics/double-buffering.h>
#include <util/font8x16.h>
#include <ctypes.h>

extern gfx_header_t gfx_h;
static volatile uint32_t *fb;

void configure_tty_utils(void)
{
    fb = (uint32_t *)gfx_h.fb_addr;
}

void plot_pix_fb(uint32_t color, int x, int y)
{
    fb[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

#include "../../serial/serial.h"

static inline bool validate_dimensions(char c, struct tty *tty)
{
    // This is super weird but if I don't subtract char_width it won't write
    // the text on a newline as it should. It detects it, and sets console.{x,y},
    // but it won't do anything until I write another char.
    if (tty->console.x == tty->fb_width-char_width)
    {
        if (tty->console.y >= tty->fb_height)
        {
            //scroll
            debug(true, "[Check 1] Need to scroll!\n");
            tty_scroll_up();
        }
        else
        {
            tty->console.x = -char_width;
            tty->console.y += char_height;
        }
    }

    if (tty->console.y >= tty->fb_height)
    {
        //scroll
        debug(true, "[Check 2] Need to scroll!\n");
        tty_scroll_up();
    }

    if (c == '\n')
    {
        tty->console.x = 0;
        tty->console.y += char_height;

        if (tty->console.y >= tty->fb_height)
        {
            //scroll
            tty_scroll_up();
        }
    }

    return (c > 0x1F); // 0x0 (NUL) - 0x1F (unit separator) are non printable as per the ascii standard
}

static inline void plot_char_fb(char c, int x, int y, struct tty *tty)
{
    if (tty->save_contents_on_draw_call)
        tty->grid[x + y * gfx_h.fb_height] = c;
    
    // bool is_printable_char = validate_dimensions(c, tty);
    // if (!is_printable_char) return;

    for (int _y = 0; _y < char_height; _y++)
    {
        for (int _x = 0; _x < char_width; _x++)
        {
            bool draw_fg = (font[(c * char_height) + _y]) & (1 << _x);
            int ypos = y + _y;
            int xpos = x + char_width - _x;

            plot_pix_fb(draw_fg ? tty->fg : tty->bg, xpos, ypos);
        }
    }
    tty->console.x += char_width;
}

void plot_char_grid(char c, int x, int y, struct tty *tty)
{
    // Perform checks to avoid redrawing to the framebuffer?
    if (!tty->override_fast_plot) {
        if (!is_verbose_boot()) goto no_render;
        
        bool fast_plot = tty->grid[x + y * tty->fb_height] == c;
        fast_plot == false ? plot_char_fb(c, x, y, tty) : (void)0;

        no_render:
            return;
    }

    // No. Overwrite the current character with 'c' regardless of whether they're same or not.
    plot_char_fb(c, x, y, tty);
}

void cls(struct tty *tty)
{
    for (int i = 0; i < tty->fb_height * tty->fb_width; i++)
    {
        plot_pix_fb(tty->bg, fb_dimensions_to_vec2(i, tty).x, fb_dimensions_to_vec2(i, tty).y);
    }
}