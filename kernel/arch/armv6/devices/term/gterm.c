/*
	Copyright 2019, 2020, 2021, 2022 mintsuki and contributors.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
	OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	-----------
	This code was taken from: https://github.com/V01D-NULL/limine-terminal-port
	which is a freestanding port of the limine terminal: https://github.com/limine-bootloader/limine
*/

#include <base/string.h>
#include <base/base-types.h>
#include <mm/addr.h>
#include "gterm.h"
#include "term.h"
#include <moon-io/serial.h>
#include <base/align.h>

#define PR_MODULE "term"
#include <printk.h>

// A simple "bump" allocator that steals some memory from the mmap
static int offset = 0;
static uint8_t *base = (uint8_t *)0x20000000;
void *bump(size_t bytes)
{
	auto ptr = base;
	offset += bytes;
	base += bytes;
	base = (uint8_t *)align((uintptr_t)base, 16);
	memset((void *)ptr, 0, bytes);
	return (void *)ptr;
}

struct term_context
{
	bool control_sequence;
	bool csi;
	bool escape;
	bool rrr;
	bool discard_next;
	bool bold;
	bool reverse_video;
	bool dec_private;
	bool insert_mode;
	uint8_t g_select;
	uint8_t charsets[2];
	size_t current_charset;
	size_t escape_offset;
	size_t esc_values_i;
	size_t saved_cursor_x;
	size_t saved_cursor_y;
	size_t current_primary;
	size_t scroll_top_margin;
	size_t scroll_bottom_margin;
	uint32_t esc_values[16];

	bool saved_state_bold;
	bool saved_state_reverse_video;
	size_t saved_state_current_charset;
	size_t saved_state_current_primary;
} term_context;

// Maximum allowed font size in bytes. 16kB should be enough as 9x32 is the
// largest font I've seen, and that would take 9*32 * 256 * 1/8 byte =
// 9216 bytes.
#define VGA_FONT_MAX 16384
#define VGA_FONT_GLYPHS 256

#define DEFAULT_FONT_WIDTH 8
#define DEFAULT_FONT_HEIGHT 8

static size_t vga_font_width;
static size_t vga_font_height;
static size_t glyph_width = 8;
static size_t glyph_height = 8;

static size_t vga_font_scale_x = 1;
static size_t vga_font_scale_y = 1;

static size_t offset_x, offset_y;

static volatile uint32_t *gterm_framebuffer;
static uint16_t gterm_pitch;
static uint16_t gterm_width;
static uint16_t gterm_height;
static uint16_t gterm_bpp;

static uint8_t *vga_font_bits = NULL;
static size_t vga_font_bool_size = 0;
static bool *vga_font_bool = NULL;

static uint32_t ansi_colours[8];
static uint32_t ansi_bright_colours[8];
static uint32_t default_fg, default_bg;

static size_t bg_canvas_size = 0;
static uint32_t *bg_canvas = NULL;

static size_t rows;
static size_t cols;
static size_t margin;
static size_t margin_gradient;

static size_t grid_size = 0;
static size_t queue_size = 0;
static size_t map_size = 0;

struct gterm_char
{
	uint32_t c;
	uint32_t fg;
	uint32_t bg;
};

static struct gterm_char *grid = NULL;

struct queue_item
{
	size_t x, y;
	struct gterm_char c;
};

static struct queue_item *queue = NULL;
static size_t queue_i = 0;

static struct queue_item **map = NULL;

static struct context
{
	uint32_t text_fg;
#define text_fg context.text_fg
	uint32_t text_bg;
#define text_bg context.text_bg
	bool cursor_status;
#define cursor_status context.cursor_status
	size_t cursor_x;
#define cursor_x context.cursor_x
	size_t cursor_y;
#define cursor_y context.cursor_y
	bool scroll_enabled;
#define scroll_enabled context.scroll_enabled

	uint32_t saved_state_text_fg;
#define saved_state_text_fg context.saved_state_text_fg
	uint32_t saved_state_text_bg;
#define saved_state_text_bg context.saved_state_text_bg
	size_t saved_state_cursor_x;
#define saved_state_cursor_x context.saved_state_cursor_x
	size_t saved_state_cursor_y;
#define saved_state_cursor_y context.saved_state_cursor_y
} context;

static size_t old_cursor_x = 0;
static size_t old_cursor_y = 0;

void gterm_save_state(void)
{
	saved_state_text_fg = text_fg;
	saved_state_text_bg = text_bg;
	saved_state_cursor_x = cursor_x;
	saved_state_cursor_y = cursor_y;
}

void gterm_restore_state(void)
{
	text_fg = saved_state_text_fg;
	text_bg = saved_state_text_bg;
	cursor_x = saved_state_cursor_x;
	cursor_y = saved_state_cursor_y;
}

void gterm_swap_palette(void)
{
	uint32_t tmp = text_bg;
	text_bg = text_fg;
	text_fg = tmp;
}

static inline void gterm_plot_px(size_t x, size_t y, uint32_t hex)
{
	auto fb = (uint8_t *)gterm_framebuffer;
	fb += CORRECT_XY(x, y);
	*fb++ = (hex >> 16);
	*fb++ = (hex >> 8) & 0xFF;
	*fb++ = hex & 0xFF;
}

static void gterm_generate_canvas(void)
{
	for (size_t y = 0; y < gterm_height; y++)
	{
		for (size_t x = 0; x < gterm_width; x++)
		{
			bg_canvas[y * gterm_width + x] = default_bg;
			gterm_plot_px(x, y, default_bg);
		}
	}
}

static void plot_char(struct gterm_char *c, size_t x, size_t y)
{
	if (x >= cols || y >= rows)
	{
		return;
	}

	x = offset_x + x * glyph_width;
	y = offset_y + y * glyph_height;
	// trace("fb_putc: %c (%d)\n", c->c, c->c);
	fb_putc(x, y, 0xffffff, c->c);

	// bool *glyph = &vga_font_bool[c->c * vga_font_height * vga_font_width];
	// // naming: fx,fy for font coordinates, gx,gy for glyph coordinates
	// for (size_t gy = 0; gy < glyph_height; gy++)
	// {
	// 	uint8_t fy = gy / vga_font_scale_y;
	// 	volatile uint32_t *fb_line = gterm_framebuffer + x + (y + gy) * (gterm_pitch / 4);
	// 	uint32_t *canvas_line = bg_canvas + x + (y + gy) * gterm_width;
	// 	for (size_t fx = 0; fx < vga_font_width; fx++)
	// 	{
	// 		bool draw = glyph[fy * vga_font_width + fx];
	// 		for (size_t i = 0; i < vga_font_scale_x; i++)
	// 		{
	// 			size_t gx = vga_font_scale_x * fx + i;
	// 			uint32_t bg = c->bg == 0xffffffff ? canvas_line[gx] : c->bg;
	// 			uint32_t fg = c->fg == 0xffffffff ? canvas_line[gx] : c->fg;
	// 			fb_line[gx] = draw ? fg : bg;
	// 		}
	// 	}
	// }
}

static void plot_char_fast(struct gterm_char *old, struct gterm_char *c, size_t x, size_t y)
{
	if (x >= cols || y >= rows)
	{
		return;
	}

	x = offset_x + x * glyph_width;
	y = offset_y + y * glyph_height;

	plot_char(c, x, y);
	// bool *new_glyph = &vga_font_bool[c->c * vga_font_height * vga_font_width];
	// bool *old_glyph = &vga_font_bool[old->c * vga_font_height * vga_font_width];
	// for (size_t gy = 0; gy < glyph_height; gy++)
	// {
	// 	uint8_t fy = gy / vga_font_scale_y;
	// 	volatile uint32_t *fb_line = gterm_framebuffer + x + (y + gy) * (gterm_pitch / 4);
	// 	uint32_t *canvas_line = bg_canvas + x + (y + gy) * gterm_width;
	// 	for (size_t fx = 0; fx < vga_font_width; fx++)
	// 	{
	// 		bool old_draw = old_glyph[fy * vga_font_width + fx];
	// 		bool new_draw = new_glyph[fy * vga_font_width + fx];
	// 		if (old_draw == new_draw)
	// 			continue;
	// 		for (size_t i = 0; i < vga_font_scale_x; i++)
	// 		{
	// 			size_t gx = vga_font_scale_x * fx + i;
	// 			uint32_t bg = c->bg == 0xffffffff ? canvas_line[gx] : c->bg;
	// 			uint32_t fg = c->fg == 0xffffffff ? canvas_line[gx] : c->fg;
	// 			fb_line[gx] = new_draw ? fg : bg;
	// 		}
	// 	}
	// }
}

static inline bool compare_char(struct gterm_char *a, struct gterm_char *b)
{
	return !(a->c != b->c || a->bg != b->bg || a->fg != b->fg);
}

static void push_to_queue(struct gterm_char *c, size_t x, size_t y)
{
	if (x >= cols || y >= rows)
	{
		return;
	}

	size_t i = y * cols + x;
	struct queue_item *q = map[i];

	if (q == NULL)
	{
		if (compare_char(&grid[i], c))
		{
			return;
		}

		q = &queue[queue_i++];
		q->x = x;
		q->y = y;
		map[i] = q;
	}

	q->c = *c;
}

bool gterm_scroll_disable(void)
{
	bool ret = scroll_enabled;
	scroll_enabled = false;
	return ret;
}

void gterm_scroll_enable(void)
{
	scroll_enabled = true;
}

void gterm_revscroll(void)
{
	for (size_t i = (term_context.scroll_bottom_margin - 1) * cols - 1;; i--)
	{
		struct gterm_char *c;
		struct queue_item *q = map[i];
		if (q != NULL)
		{
			c = &q->c;
		}
		else
		{
			c = &grid[i];
		}
		push_to_queue(c, (i + cols) % cols, (i + cols) / cols);
		if (i == term_context.scroll_top_margin * cols)
		{
			break;
		}
	}

	// Clear the first line of the screen.
	struct gterm_char empty;
	empty.c = ' ';
	empty.fg = text_fg;
	empty.bg = text_bg;
	for (size_t i = term_context.scroll_top_margin * cols;
		 i < (term_context.scroll_top_margin + 1) * cols; i++)
	{
		push_to_queue(&empty, i % cols, i / cols);
	}
}

void gterm_scroll(void)
{
	term_context.scroll_bottom_margin = rows;
	term_context.scroll_top_margin = 0;
	trace("t_ctx: (top: %d, bot: %d)\n", term_context.scroll_top_margin, term_context.scroll_bottom_margin);
	for (size_t i = (term_context.scroll_top_margin + 1) * cols;
		 i < term_context.scroll_bottom_margin * cols; i++)
	{
		struct gterm_char *c;
		struct queue_item *q = map[i];
		if (q != NULL)
		{
			c = &q->c;
		}
		else
		{
			c = &grid[i];
		}
		push_to_queue(c, (i - cols) % cols, (i - cols) / cols);
	}

	// Clear the last line of the screen.
	struct gterm_char empty;
	empty.c = ' ';
	empty.fg = text_fg;
	empty.bg = text_bg;
	for (size_t i = (term_context.scroll_bottom_margin - 1) * cols;
		 i < term_context.scroll_bottom_margin * cols; i++)
	{
		push_to_queue(&empty, i % cols, i / cols);
	}
}

void gterm_clear(bool move)
{
	struct gterm_char empty;
	empty.c = ' ';
	empty.fg = text_fg;
	empty.bg = text_bg;
	for (size_t i = 0; i < rows * cols; i++)
	{
		push_to_queue(&empty, i % cols, i / cols);
	}

	if (move)
	{
		cursor_x = 0;
		cursor_y = 1;
	}
}

void gterm_enable_cursor(void)
{
	cursor_status = true;
}

bool gterm_disable_cursor(void)
{
	bool ret = cursor_status;
	cursor_status = false;
	return ret;
}

void gterm_set_cursor_pos(size_t x, size_t y)
{
	if (x >= cols)
	{
		if ((int)x < 0)
		{
			x = 0;
		}
		else
		{
			x = cols - 1;
		}
	}
	if (y >= rows)
	{
		if ((int)y < 0)
		{
			y = 0;
		}
		else
		{
			y = rows - 1;
		}
	}
	cursor_x = x;
	cursor_y = y;
}

void gterm_get_cursor_pos(size_t *x, size_t *y)
{
	*x = cursor_x;
	*y = cursor_y;
}

void gterm_move_character(size_t new_x, size_t new_y, size_t old_x, size_t old_y)
{
	if (old_x >= cols || old_y >= rows || new_x >= cols || new_y >= rows)
	{
		return;
	}

	size_t i = old_x + old_y * cols;

	struct gterm_char *c;
	struct queue_item *q = map[i];
	if (q != NULL)
	{
		c = &q->c;
	}
	else
	{
		c = &grid[i];
	}

	push_to_queue(c, new_x, new_y);
}

void gterm_set_text_fg(size_t fg)
{
	text_fg = ansi_colours[fg];
}

void gterm_set_text_bg(size_t bg)
{
	text_bg = ansi_colours[bg];
}

void gterm_set_text_fg_bright(size_t fg)
{
	text_fg = ansi_bright_colours[fg];
}

void gterm_set_text_bg_bright(size_t bg)
{
	text_bg = ansi_bright_colours[bg];
}

void gterm_set_text_fg_default(void)
{
	text_fg = default_fg;
}

void gterm_set_text_bg_default(void)
{
	text_bg = 0xffffffff;
}

static void draw_cursor(void)
{
	size_t i = cursor_x + cursor_y * cols;
	struct gterm_char c;
	struct queue_item *q = map[i];
	if (q != NULL)
	{
		c = q->c;
	}
	else
	{
		c = grid[i];
	}
	uint32_t tmp = c.fg;
	c.fg = c.bg;
	c.bg = tmp;
	plot_char(&c, cursor_x, cursor_y);
	if (q != NULL)
	{
		grid[i] = q->c;
		map[i] = NULL;
	}
}

static bool init = false;
void gterm_double_buffer_flush(void)
{
	if (cursor_status)
	{
		draw_cursor();
	}

	// trace("queue i = %d\n");
	for (size_t i = 0; i < queue_i; i++)
	{
		struct queue_item *q = &queue[i];
		size_t offset = q->y * cols + q->x;
		if (map[offset] == NULL)
		{
			// trace("map[offset (%d)] is NULL\n", offset);
			continue;
		}
		struct gterm_char *old = &grid[offset];
		if (q->c.bg == old->bg && q->c.fg == old->fg)
		{
			// trace("plot_cf: %c x: %d y: %d\n", q->c.c, q->x, q->y);
			// if (init)
			// {
			// 	trace("plot_cf: %d x: %d y: %d (q=%p)\n", q->c.c, q->x, q->y, q);
			// 	trace("plot_cf: corrected (x, y): (%d, %d)\n", CORRECT_X(q->x), CORRECT_Y(q->y));
			// }
			plot_char(&q->c, q->x, q->y);
			// plot_char_fast(old, &q->c, q->x, q->y);
		}
		else
		{
			// fb_putc(q->x, q->y, q->c.fg, q->c.c);
			// if (init)
			// {
			// 	trace("plot_c: %d x: %d y: %d (q=%p)\n", q->c.c, q->x, q->y, q);
			// 	trace("plot_c: corrected (x, y): (%d, %d)\n", CORRECT_X(q->x), CORRECT_Y(q->y));
			// }
			plot_char(&q->c, q->x, q->y);
		}
		grid[offset] = q->c;
		map[offset] = NULL;

		if (init)
		{
			// trace("\nqueue_i is %d\n", queue_i);
			// for (;;)
			// 	;
		}
	}

	// if ((old_cursor_x != cursor_x || old_cursor_y != cursor_y) || cursor_status == false)
	// {
	// 	plot_char(&grid[old_cursor_x + old_cursor_y * cols], old_cursor_x, old_cursor_y);
	// }

	old_cursor_x = cursor_x;
	old_cursor_y = cursor_y;

	queue_i = 0;
	init = true;
}

void gterm_putchar(uint8_t c)
{
	struct gterm_char ch;
	ch.c = c;
	ch.fg = text_fg;
	ch.bg = text_bg;
	push_to_queue(&ch, cursor_x++, cursor_y);
	if (cursor_x == cols && (cursor_y < term_context.scroll_bottom_margin - 1 || scroll_enabled))
	{
		cursor_x = 0;
		cursor_y++;
	}
	if (cursor_y == term_context.scroll_bottom_margin)
	{
		cursor_y--;
		gterm_scroll();
	}
}

bool gterm_prepare(void)
{
	cursor_status = true;
	scroll_enabled = true;

	// default scheme
	margin = 0;
	margin_gradient = 0;

	default_bg = 0x00000000; // background (black)
	default_fg = 0x00aaaaaa; // foreground (white) For gray use: 00aaaaaa

	text_fg = default_fg;
	text_bg = default_bg;

	gterm_framebuffer = (volatile uint32_t *)0x18300000;
	gterm_width = FB_TOP_WIDTH;
	gterm_height = FB_HEIGHT;
	gterm_bpp = 8;
	gterm_pitch = FB_TOP_WIDTH * gterm_bpp;

	vga_font_width = DEFAULT_FONT_WIDTH, vga_font_height = DEFAULT_FONT_HEIGHT;

	size_t font_spacing = 1;
	vga_font_width += font_spacing;

	vga_font_scale_x = 1;
	vga_font_scale_y = 1;

	glyph_width = vga_font_width * vga_font_scale_x;
	glyph_height = vga_font_height * vga_font_scale_y;

	cols = (gterm_width / glyph_width);
	rows = (gterm_height / glyph_height) - 1;

	offset_x = 1;
	cursor_y = offset_y = 1;

	grid_size = rows * cols * sizeof(struct gterm_char);
	grid = bump(grid_size);
	trace("Grid = %x\n", grid);

	queue_size = rows * cols * sizeof(struct queue_item);
	queue = bump(queue_size);
	queue_i = 0;
	trace("Queue: %x\n", queue);

	map_size = rows * cols * sizeof(struct queue_item *);
	map = bump(map_size);
	trace("Map: %x\n", map);

	bg_canvas_size = gterm_width * gterm_height * sizeof(uint32_t);
	bg_canvas = bump(bg_canvas_size);
	trace("bg canvas: %x\n", bg_canvas);

	return true;
}

void gterm_init(void)
{
	gterm_generate_canvas();
	gterm_clear(true);
	gterm_double_buffer_flush();
}

uint64_t gterm_context_size(void)
{
	uint64_t ret = 0;

	ret += sizeof(struct context);
	ret += grid_size;

	return ret;
}

void gterm_context_save(uint64_t ptr)
{
	memcpy64((uint64_t *)ptr, (uint64_t *)(uintptr_t)&context, sizeof(struct context));
	ptr += sizeof(struct context);

	memcpy64((uint64_t *)ptr, (uint64_t *)(uintptr_t)grid, grid_size);
}

void gterm_context_restore(uint64_t ptr)
{
	memcpy64((uint64_t *)(uintptr_t)&context, (uint64_t *)ptr, sizeof(struct context));
	ptr += sizeof(struct context);

	memcpy64((uint64_t *)(uintptr_t)grid, (uint64_t *)ptr, grid_size);

	for (size_t i = 0; i < (size_t)rows * cols; i++)
	{
		size_t x = i % cols;
		size_t y = i / cols;

		plot_char(&grid[i], x, y);
	}

	if (cursor_status)
	{
		draw_cursor();
	}
}

void gterm_full_refresh(void)
{
	gterm_generate_canvas();

	for (size_t i = 0; i < (size_t)rows * cols; i++)
	{
		size_t x = i % cols;
		size_t y = i / cols;

		plot_char(&grid[i], x, y);
	}

	if (cursor_status)
	{
		draw_cursor();
	}
}