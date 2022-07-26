#ifndef EARLY_FB_H
#define EARLY_FB_H

#include <stdint.h>
#include <boot/boot.h>
#include <libgraphics/draw.h>

// 1920 * 1080 * 20 bytes have been reserved in the linker script
// to aid in early framebuffer/double buffer or console initialization.
// fb_1080x1920_screen_malloc() returns this memory if available (returns NULL upon failure).
void *fb_1080x1920_screen_malloc(uint64_t size);
void early_fb_init(BootContext boot);
void fb_draw_image(int x1, int y1, int x2, int y2, const uint8_t *image, image_type_t img_type);

#endif // EARLY_FB_H