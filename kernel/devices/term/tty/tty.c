/*
    tty design heavily inspired by https://github.com/qword-os/qword
*/
#include "tty.h"
#include "tty.utils.h"
#include <libgraphics/double-buffering.h>
#include <libgraphics/bootsplash.h>
#include <libk/kstring.h>
#include <util/font8x16.h>
#include <devices/serial/serial.h>

static struct tty tty;
static bool verbose_boot = false;

extern gfx_header_t gfx_h;

void tty_redraw(void);
void verbose_boot_irq(isr_t isr)
{
    // debug(true, "vboot irq\n");
    // if (!verbose_boot)
    // {
    //     tty_clear_fast();
    //     tty_redraw();
    //     for (int i = 0; i < tty.fb_height * tty.fb_width; i++) {
    //     debug(false, "%c", tty.grid[i]);
    //     }
    // }
    // else
    // {
    //     bootsplash();
    // }

    verbose_boot = !verbose_boot;
}

bool is_verbose_boot(void) { return verbose_boot; }

void tty_init(void)
{
    configure_tty_utils();
    tty.grid = (char*)double_buffering_create_buffer();
    tty.fg = 0xFFFFFF;
    tty.bg = 0x2278C2;

    // Width and height of the framebuffer (in pixels)
    tty.fb_width = gfx_h.fb_width;
    tty.fb_height = gfx_h.fb_height;
    
    // Number of printable characters until the
    // edge of the screen/framebuffer is reached
    tty.tty_width = gfx_h.fb_width / char_width;
    tty.tty_height = gfx_h.fb_height / char_height;

    tty.override_fast_plot = false;
    tty.save_contents_on_draw_call = true;

    if (tty.bg != 0)
        tty_clear_fast();

    debug(true, "tty_width: %d chars\n", tty.tty_width);
    debug(true, "tty_height: %d chars\n", tty.tty_height);
}

void tty_putc(char c)
{
    // This is super weird but if I don't subtract char_width it won't write
    // the text on a newline as it should. It detects it, and sets console.{x,y},
    // but it won't do anything until I write another char.
    bool scrolled = false;

    if (tty.console.x == tty.fb_width-char_width)
    {
        if (tty.console.y >= tty.fb_height)
        {
            //scroll
            tty_scroll_up();
            tty.console.x = 0;
            tty.console.y = tty.tty_height-1;
            scrolled = true;
        }
        else
        {
            tty.console.x = -char_width;
            tty.console.y += char_height;
        }
    }

    if (tty.console.y >= tty.fb_height)
    {
        //scroll
        if (!scrolled) {
            tty_scroll_up();
            
            tty.console.x = 0;
            tty.console.y = tty.tty_height-1;
            scrolled = true;
        }
    }

    if (c == '\n')
    {
        tty.console.x = 0;
        tty.console.y += char_height;

        if (tty.console.y >= tty.fb_height)
        {
            if (!scrolled) {
                tty_scroll_up();
                tty.console.x = 0;
                tty.console.y = tty.tty_height-1;
            }
        }
    }

    bool is_printable_char = c > 0x1F; // 0x0 (NUL) - 0x1F (unit separator) are non printable as per the ascii standard
    if (is_printable_char) {
        plot_char_grid(c, tty.console.x, tty.console.y, &tty);
    }
}

void tty_puts(const char *str)
{
    for (size_t i = 0, n = strlen(str); i < n; i++)
    {
        tty_putc(
            str[i]
        );
    }
}

void tty_clear(void)
{
    tty.save_contents_on_draw_call = false;
    tty.override_fast_plot = true;
    for (int i = 0; i < tty.fb_height * tty.fb_width; i++) {
        plot_char_grid(
            ' ',
            fb_dimensions_to_vec2(i, &tty).x,
            fb_dimensions_to_vec2(i, &tty).y,
            &tty
        );
    }
    tty.save_contents_on_draw_call = true;
    tty.override_fast_plot = false;
}

// tty_clear_fast renders tty.bg directly to the framebuffer using plot_pix_fb
void tty_clear_fast(void)
{
    cls(&tty);
}

void tty_redraw(void)
{
    tty.save_contents_on_draw_call = false;
    tty.override_fast_plot = true;
    
    /* It's safe to assume that the contents of the framebuffer can be overwritten in tty_redraw() */
    tty.console.x = 0;
    tty.console.y = 0;

    for (int i = 0; i < tty.fb_height * tty.fb_width; i++) {
        plot_char_grid(
            tty.grid[i],
            tty.console.x,
            tty.console.y,
            &tty
        );
    }
    tty.save_contents_on_draw_call = true;
    tty.override_fast_plot = false;
}

void tty_scroll_up(void)
{
    for (int i = tty.fb_height * tty.fb_width; i > 0; i--)
    {
        plot_char_grid(
            tty.grid[i],
            i % tty.fb_width,
            i / tty.fb_width,
            &tty
        );
    }
}