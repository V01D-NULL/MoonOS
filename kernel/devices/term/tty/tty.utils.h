#ifndef TTY_UTILS_H
#define TTY_UTILS_H

#include <stdint.h>
#include <ctypes.h>
#include "tty.h"

void configure_tty_utils(void);
void plot_pix_fb(uint32_t color, int x, int y);
void plot_char_grid(char c, int x, int y, struct tty *tty);
void cls(struct tty *tty);

// Converts an index on the framebuffer into cartesian coordinates
static inline struct vec2i fb_dimensions_to_vec2(int fb_dimension, struct tty *tty)
{
    return (struct vec2i) {
        .x = fb_dimension % tty->fb_width,
        .y = fb_dimension / tty->fb_width
    };
}

#endif // TTY_UTILS_H