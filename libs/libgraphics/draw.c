#include "draw.h"
#include "double-buffering.h"
#include <drivers/io/serial.h>

void draw_image(int x1, int y1, int x2, int y2, const uint8_t *image, image_type_t img_type)
{
    /* RGB is the only supported format as of now */
    if (img_type != IMAGE_RGB)
        return;

    int j = 0;
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            uint32_t pixel = image[j] << 16 | image[j + 1] << 8 | image[j + 2];
            buffer_pixel(x, y, pixel);
            j += 3;
        }
    }

    swap_buffers();
}

void fill_rect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            buffer_pixel(x, y, color);
        }
    }

    swap_buffers();
}