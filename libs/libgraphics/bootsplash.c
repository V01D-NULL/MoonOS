#include "bootsplash.h"
#include "bootsplash_img.h"
#include "double-buffering.h"
#include "draw.h"

extern gfx_header_t gfx_h;

void bootsplash()
{
    flush_back_buffer(NULL);
    draw_image((gfx_h.fb_width / 2) - (IMG_WIDTH / 2), (gfx_h.fb_height / 2) - (IMG_HEIGHT / 2), IMG_WIDTH, IMG_HEIGHT, IMG_DATA, IMAGE_RGB);
}