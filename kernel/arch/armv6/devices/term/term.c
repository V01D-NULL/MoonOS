#include "term.h"
#include <framebuffer/fb.h>

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
			y += 9;
		}
		
		if (str[i] == '\t')
		{
			x += 24;
			continue;
		}

		else if (str[i] == '\n')
		{
			x = 0;
			y += 9;
			continue;
		}
		
		fb_putc(x, y, 0xFFFFFF, str[i]);
	}
}
