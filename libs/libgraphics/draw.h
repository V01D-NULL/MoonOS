#ifndef LIB_GRAPHICS_DRAW_H
#define LIB_GRAPHICS_DRAW_H

#include <stdint.h>

typedef enum {
    IMAGE_RGB = 0
} image_type_t;

void draw_image(int x1, int y1, int x2, int y2, const uint8_t *image, image_type_t img_type);
void fill_rect(int x1, int y1, int x2, int y2, int color);

#endif // LIB_GRAPHICS_DRAW_H