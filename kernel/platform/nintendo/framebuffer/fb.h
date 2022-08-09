#ifndef FB_H
#define FB_H

#include <base/base-types.h>

/*
	References:
	 - https://www.3dbrew.org/wiki/GPU/FR
	 - https://www.3dbrew.org/wiki/MTX_Registers
*/
#define FB_TOP_WIDTH     400
#define FB_BOTTOM_WIDTH  320
#define FB_HEIGHT        240 // Both displays have the same height

// The physical display is rotated by 90° meaning (0,0) is the bottom left.
// These helper macros relocate the (x,y) offsets so that the (x,y) coordinates
// are relative to the top left corner of the display
#define LCD_SKIP 3
#define CORRECT_X(x) (x * FB_HEIGHT * 3)
#define CORRECT_Y(y) (FB_HEIGHT - y) * LCD_SKIP
#define CORRECT_XY(x, y) CORRECT_X(x) + CORRECT_Y(y)

void fb_putc(uint32_t posX, uint32_t posY, uint32_t color, char c);

#endif // FB_H