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
#ifndef __LIB__GTERM_H__
#define __LIB__GTERM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stivale2.h>

void gterm_init(void);
bool gterm_prepare(size_t *_rows, size_t *_cols, struct stivale2_struct_tag_framebuffer *fb, struct stivale2_struct_tag_memmap *mmap);

void gterm_putchar(uint8_t c);
void gterm_clear(bool move);
void gterm_enable_cursor(void);
bool gterm_disable_cursor(void);
void gterm_set_cursor_pos(size_t x, size_t y);
void gterm_get_cursor_pos(size_t *x, size_t *y);
void gterm_set_text_fg(size_t fg);
void gterm_set_text_bg(size_t bg);
void gterm_set_text_fg_bright(size_t fg);
void gterm_set_text_bg_bright(size_t bg);
void gterm_set_text_fg_default(void);
void gterm_set_text_bg_default(void);
bool gterm_scroll_disable(void);
void gterm_scroll_enable(void);
void gterm_move_character(size_t new_x, size_t new_y, size_t old_x, size_t old_y);
void gterm_scroll(void);
void gterm_revscroll(void);
void gterm_swap_palette(void);
void gterm_save_state(void);
void gterm_restore_state(void);

void gterm_double_buffer_flush(void);

uint64_t gterm_context_size(void);
void gterm_context_save(uint64_t ptr);
void gterm_context_restore(uint64_t ptr);
void gterm_full_refresh(void);

#endif