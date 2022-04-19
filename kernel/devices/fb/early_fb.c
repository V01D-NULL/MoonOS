#include "early_fb.h"
#include <amd64/moon.h>
#include <mm/mm.h>
#define HEAP_SZ 1920 * 1080 * 20

EXTERNAL(__embedded_kheap_start);
EXTERNAL(__embedded_kheap_end);

static uint64_t heap_ptr = 0;

static uint32_t *fb = NULL;
static uint16_t fb_width, fb_height;

void early_fb_init(BootContext boot)
{
    fb = (uint32_t*)boot.fb.fb_addr;
    fb_width = boot.fb.fb_width;
    fb_height = boot.fb.fb_height;
}

void *fb_1080x1920_screen_malloc(uint64_t size)
{
    uint64_t heap_base = (uint64_t)__embedded_kheap_start;

    if (size > HEAP_SZ || size <= 0 || heap_base + heap_ptr > HEAP_SZ)
        return NULL;

    void *mem = __embedded_kheap_start + heap_ptr;
    heap_ptr += size;
    return mem;
}

void fb_draw_image(int x1, int y1, int x2, int y2, const uint8_t *image, image_type_t img_type)
{
    /* RGB is the only supported format as of now */
    if (img_type != IMAGE_RGB)
        return;

    int j = 0;
    for (int y = y1; y < (y2 + y1); y++)
    {
        for (int x = x1; x < (x2 + x1); x++)
        {
            uint32_t pixel = image[j] << 16 | image[j + 1] << 8 | image[j + 2];
            fb[x + y * fb_width] = pixel;
            j += 3;
        }
    }
}