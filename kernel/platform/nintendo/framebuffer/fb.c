#include <platform/nintendo/font.h>
#include <moon.h>
#include "fb.h"

inline void pixel(uint8_t *fb, uint32_t hex, uint32_t x, uint32_t y)
{
	fb += CORRECT_XY(x, y);
	*fb++ = (hex >> 16);
	*fb++ = (hex >> 8) & 0xFF;
	*fb++ = hex & 0xFF;
}

void fb_putc(uint32_t posX, uint32_t posY, uint32_t color, char c)
{
	uint8_t *top_display_fb = (uint8_t *)0x18300000;

	for (int y = 0; y < char_height; y++)
	{
		uint8_t selection = font[c * char_height + y];
		for (int x = 0; x < char_width; x++)
		{
			bool can_draw = selection & (1 << (7 - x));
			if (can_draw)
				pixel(top_display_fb, color, posX + x, posY + y);
			else
				pixel(top_display_fb, 0, posX + x, posY + y);
		}
	}
}
