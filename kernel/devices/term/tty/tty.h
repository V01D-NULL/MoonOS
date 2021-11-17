#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <int/interrupts.h>

struct vec2i
{
    int x, y;
};

struct tty
{
    char *grid;
    uint32_t fg;
    uint32_t bg;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t tty_width;
    uint16_t tty_height;
    struct vec2i console; // x & y offsets in pixels, not chars
    bool override_fast_plot; //If true there is no check to avoid redrawing the same glyph to save time. This should be true for clear, otherwise false.
    bool save_contents_on_draw_call; // Determines whether the next draw call should save it's contents in the grid variable. Should be false for clear, otherwise true.
    bool ignore_bound_checking;
};

void tty_init(void);
void tty_putc(char c);
void tty_puts(const char *str);
void tty_clear(void);
void tty_clear_fast(void);
void tty_scroll_up(void);

void verbose_boot_irq(isr_t isr);
bool is_verbose_boot(void);

#endif // TTY_H