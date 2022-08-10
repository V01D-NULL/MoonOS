#include "term.h"
#include <framebuffer/fb.h>
#include <font.h>

static int x = 1;
static int y = 1;

// Crude console hack for now.
void _term_write(string_view str, size_t len)
{
	for (auto i = 0ul; i < len; i++, x += 8)
	{
		if (x >= FB_TOP_WIDTH)
		{
			x = 0;
			y += char_height;
		}
		
		if (str[i] == '\t')
		{
			x += char_width * 2;
			continue;
		}

		else if (str[i] == '\n')
		{
			x = 0;
			y += char_height;
			continue;
		}
		
		fb_putc(x, y, 0xFFFFFF, str[i]);
	}
}
