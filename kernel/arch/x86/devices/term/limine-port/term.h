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
#ifndef __LIB__TERM_H__
#define __LIB__TERM_H__

#include <base/base-types.h>
#include "stivale2.h"

#define TERM_TABSIZE (8)
#define MAX_ESC_VALUES (16)

extern struct term_context {
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
    uint32_t esc_values[MAX_ESC_VALUES];

    bool saved_state_bold;
    bool saved_state_reverse_video;
    size_t saved_state_current_charset;
    size_t saved_state_current_primary;
} term_context;

enum {
    NOT_READY,
    VBE
};

extern int term_backend;
extern size_t term_rows, term_cols;
extern bool term_runtime;

void term_reinit(void);
void term_deinit(void);
void term_init(void);
void term_prepare(struct stivale2_struct_tag_framebuffer *fb, struct stivale2_struct_tag_memmap *mmap);
void term_notready(void);
void term_putchar(uint8_t c);
void _term_write(string_view buf, uint64_t count);

extern void (*raw_putchar)(uint8_t c);
extern void (*clear)(bool move);
extern void (*enable_cursor)(void);
extern bool (*disable_cursor)(void);
extern void (*set_cursor_pos)(size_t x, size_t y);
extern void (*get_cursor_pos)(size_t *x, size_t *y);
extern void (*set_text_fg)(size_t fg);
extern void (*set_text_bg)(size_t bg);
extern void (*set_text_fg_bright)(size_t fg);
extern void (*set_text_bg_bright)(size_t bg);
extern void (*set_text_fg_default)(void);
extern void (*set_text_bg_default)(void);
extern bool (*scroll_disable)(void);
extern void (*scroll_enable)(void);
extern void (*term_move_character)(size_t new_x, size_t new_y, size_t old_x, size_t old_y);
extern void (*term_scroll)(void);
extern void (*term_revscroll)(void);
extern void (*term_swap_palette)(void);
extern void (*term_save_state)(void);
extern void (*term_restore_state)(void);

extern void (*term_double_buffer_flush)(void);

extern uint64_t (*term_context_size)(void);
extern void (*term_context_save)(uint64_t ptr);
extern void (*term_context_restore)(uint64_t ptr);
extern void (*term_full_refresh)(void);

#define TERM_CB_DEC 10
#define TERM_CB_BELL 20
#define TERM_CB_PRIVATE_ID 30
#define TERM_CB_STATUS_REPORT 40
#define TERM_CB_POS_REPORT 50
#define TERM_CB_KBD_LEDS 60
#define TERM_CB_MODE 70
#define TERM_CB_LINUX 80

#define TERM_CTX_SIZE ((uint64_t)(-1))
#define TERM_CTX_SAVE ((uint64_t)(-2))
#define TERM_CTX_RESTORE ((uint64_t)(-3))
#define TERM_FULL_REFRESH ((uint64_t)(-4))

extern uint64_t term_arg;
extern void (*term_callback)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

#endif