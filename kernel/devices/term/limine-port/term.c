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
#include "term.h"
#include "gterm.h"

static bool gterm_ok = false;

void term_deinit(void) {
    term_notready();
}

void term_prepare(struct stivale2_struct_tag_framebuffer *fb, struct stivale2_struct_tag_memmap *mmap) {
    gterm_ok = gterm_prepare(&term_rows, &term_cols, fb, mmap);
}

void term_init(void)
{
    if (!gterm_ok)
        return;
    
    gterm_init();
    term_reinit();

    raw_putchar    = gterm_putchar;
    clear          = gterm_clear;
    enable_cursor  = gterm_enable_cursor;
    disable_cursor = gterm_disable_cursor;
    set_cursor_pos = gterm_set_cursor_pos;
    get_cursor_pos = gterm_get_cursor_pos;
    set_text_fg    = gterm_set_text_fg;
    set_text_bg    = gterm_set_text_bg;
    set_text_fg_bright = gterm_set_text_fg_bright;
    set_text_bg_bright = gterm_set_text_bg_bright;
    set_text_fg_default = gterm_set_text_fg_default;
    set_text_bg_default = gterm_set_text_bg_default;
    scroll_disable = gterm_scroll_disable;
    scroll_enable  = gterm_scroll_enable;
    term_move_character = gterm_move_character;
    term_scroll = gterm_scroll;
    term_revscroll = gterm_revscroll;
    term_swap_palette = gterm_swap_palette;
    term_save_state = gterm_save_state;
    term_restore_state = gterm_restore_state;

    term_double_buffer_flush = gterm_double_buffer_flush;

    term_context_size = gterm_context_size;
    term_context_save = gterm_context_save;
    term_context_restore = gterm_context_restore;
    term_full_refresh = gterm_full_refresh;

    term_backend = VBE;
}


// Tries to implement this standard for terminfo
// https://man7.org/linux/man-pages/man4/console_codes.4.html

int term_backend = NOT_READY;
size_t term_rows, term_cols;
bool term_runtime = false;

static void notready_raw_putchar(uint8_t c) {
    (void)c;
}
static void notready_clear(bool move) {
    (void)move;
}
static void notready_void(void) {}
static void notready_set_cursor_pos(size_t x, size_t y) {
    (void)x; (void)y;
}
static void notready_get_cursor_pos(size_t *x, size_t *y) {
    *x = 0;
    *y = 0;
}
static void notready_size_t(size_t n) {
    (void)n;
}
static bool notready_disable(void) {
    return false;
}
static void notready_move_character(size_t a, size_t b, size_t c, size_t d) {
    (void)a; (void)b; (void)c; (void)d;
}
static uint64_t notready_context_size(void) {
    return 0;
}
static void notready_uint64_t(uint64_t n) {
    (void)n;
}

void term_notready(void) {
    term_backend = NOT_READY;

    raw_putchar = notready_raw_putchar;
    clear = notready_clear;
    enable_cursor = notready_void;
    disable_cursor = notready_disable;
    set_cursor_pos = notready_set_cursor_pos;
    get_cursor_pos = notready_get_cursor_pos;
    set_text_fg = notready_size_t;
    set_text_bg = notready_size_t;
    set_text_fg_bright = notready_size_t;
    set_text_bg_bright = notready_size_t;
    set_text_fg_default = notready_void;
    set_text_bg_default = notready_void;
    scroll_disable = notready_disable;
    scroll_enable = notready_void;
    term_move_character = notready_move_character;
    term_scroll = notready_void;
    term_revscroll = notready_void;
    term_swap_palette = notready_void;
    term_save_state = notready_void;
    term_restore_state = notready_void;
    term_double_buffer_flush = notready_void;
    term_context_size = notready_context_size;
    term_context_save = notready_uint64_t;
    term_context_restore = notready_uint64_t;
    term_full_refresh = notready_void;

    term_cols = 80;
    term_rows = 24;
}

void (*raw_putchar)(uint8_t c);
void (*clear)(bool move);
void (*enable_cursor)(void);
bool (*disable_cursor)(void);
void (*set_cursor_pos)(size_t x, size_t y);
void (*get_cursor_pos)(size_t *x, size_t *y);
void (*set_text_fg)(size_t fg);
void (*set_text_bg)(size_t bg);
void (*set_text_fg_bright)(size_t fg);
void (*set_text_bg_bright)(size_t bg);
void (*set_text_fg_default)(void);
void (*set_text_bg_default)(void);
bool (*scroll_disable)(void);
void (*scroll_enable)(void);
void (*term_move_character)(size_t new_x, size_t new_y, size_t old_x, size_t old_y);
void (*term_scroll)(void);
void (*term_revscroll)(void);
void (*term_swap_palette)(void);
void (*term_save_state)(void);
void (*term_restore_state)(void);

void (*term_double_buffer_flush)(void);

uint64_t (*term_context_size)(void);
void (*term_context_save)(uint64_t ptr);
void (*term_context_restore)(uint64_t ptr);
void (*term_full_refresh)(void);

uint64_t term_arg = 0;
void (*term_callback)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = NULL;

struct term_context term_context;

#define escape_offset term_context.escape_offset
#define control_sequence term_context.control_sequence
#define csi term_context.csi
#define escape term_context.escape
#define rrr term_context.rrr
#define discard_next term_context.discard_next
#define bold term_context.bold
#define reverse_video term_context.reverse_video
#define dec_private term_context.dec_private
#define esc_values term_context.esc_values
#define esc_values_i term_context.esc_values_i
#define saved_cursor_x term_context.saved_cursor_x
#define saved_cursor_y term_context.saved_cursor_y
#define current_primary term_context.current_primary
#define insert_mode term_context.insert_mode
#define scroll_top_margin term_context.scroll_top_margin
#define scroll_bottom_margin term_context.scroll_bottom_margin
#define current_charset term_context.current_charset
#define charsets term_context.charsets
#define g_select term_context.g_select

#define saved_state_bold term_context.saved_state_bold
#define saved_state_reverse_video term_context.saved_state_reverse_video
#define saved_state_current_charset term_context.saved_state_current_charset
#define saved_state_current_primary term_context.saved_state_current_primary

#define CHARSET_DEFAULT 0
#define CHARSET_DEC_SPECIAL 1

void term_reinit(void) {
    escape_offset = 0;
    control_sequence = false;
    csi = false;
    escape = false;
    rrr = false;
    discard_next = false;
    bold = false;
    reverse_video = false;
    dec_private = false;
    esc_values_i = 0;
    saved_cursor_x = 0;
    saved_cursor_y = 0;
    current_primary = (size_t)-1;
    insert_mode = false;
    scroll_top_margin = 0;
    scroll_bottom_margin = term_rows;
    current_charset = 0;
    g_select = 0;
    charsets[0] = CHARSET_DEFAULT;
    charsets[1] = CHARSET_DEC_SPECIAL;
}

static uint64_t context_size(void) {
    uint64_t ret = 0;

    ret += sizeof(struct term_context);
    ret += term_context_size();

    return ret;
}

static void context_save(uint64_t ptr) {
    memcpy64((uint64_t*)ptr, (uint64_t*)(uintptr_t)&term_context, sizeof(struct term_context));
    ptr += sizeof(struct term_context);

    term_context_save(ptr);
}

static void context_restore(uint64_t ptr) {
    memcpy64((uint64_t*)(uintptr_t)&term_context, (uint64_t*)ptr, sizeof(struct term_context));
    ptr += sizeof(struct term_context);

    term_context_restore(ptr);
}

void _term_write(string_view buf, uint64_t count) {
    if (term_backend == NOT_READY)
        return;

    switch (count) {
        case TERM_CTX_SIZE: {
            uint64_t ret = context_size();
            memcpy64((uint64_t*)buf, (uint64_t*)(uintptr_t)&ret, sizeof(uint64_t));
            return;
        }
        case TERM_CTX_SAVE: {
            context_save((uint64_t)buf);
            return;
        }
        case TERM_CTX_RESTORE: {
            context_restore((uint64_t)buf);
            return;
        }
        case TERM_FULL_REFRESH: {
            term_full_refresh();
            return;
        }
    }

    bool native = true;

    if (!term_runtime || native) {
        // string_view s = (string_view )(uintptr_t)buf;

        for (size_t i = 0; i < count; i++)
            term_putchar(buf[i]);
    }

    term_double_buffer_flush();
}

static void sgr(void) {
    size_t i = 0;

    if (!esc_values_i)
        goto def;

    for (; i < esc_values_i; i++) {
        size_t offset;

        if (esc_values[i] == 0) {
def:
            if (reverse_video) {
                reverse_video = false;
                term_swap_palette();
            }
            bold = false;
            current_primary = (size_t)-1;
            set_text_bg_default();
            set_text_fg_default();
            continue;
        }

        else if (esc_values[i] == 1) {
            bold = true;
            if (current_primary != (size_t)-1) {
                if (!reverse_video) {
                    set_text_fg_bright(current_primary);
                } else {
                    set_text_bg_bright(current_primary);
                }
            }
            continue;
        }

        else if (esc_values[i] == 22) {
            bold = false;
            if (current_primary != (size_t)-1) {
                if (!reverse_video) {
                    set_text_fg(current_primary);
                } else {
                    set_text_bg(current_primary);
                }
            }
            continue;
        }

        else if (esc_values[i] >= 30 && esc_values[i] <= 37) {
            offset = 30;
            current_primary = esc_values[i] - offset;

            if (reverse_video) {
                goto set_bg;
            }

set_fg:
            if (bold && !reverse_video) {
                set_text_fg_bright(esc_values[i] - offset);
            } else {
                set_text_fg(esc_values[i] - offset);
            }
            continue;
        }

        else if (esc_values[i] >= 40 && esc_values[i] <= 47) {
            offset = 40;
            if (reverse_video) {
                goto set_fg;
            }

set_bg:
            if (bold && reverse_video) {
                set_text_bg_bright(esc_values[i] - offset);
            } else {
                set_text_bg(esc_values[i] - offset);
            }
            continue;
        }

        else if (esc_values[i] >= 90 && esc_values[i] <= 97) {
            offset = 90;
            current_primary = esc_values[i] - offset;

            if (reverse_video) {
                goto set_bg_bright;
            }

set_fg_bright:
            set_text_fg_bright(esc_values[i] - offset);
            continue;
        }

        else if (esc_values[i] >= 100 && esc_values[i] <= 107) {
            offset = 100;
            if (reverse_video) {
                goto set_fg_bright;
            }

set_bg_bright:
            set_text_bg_bright(esc_values[i] - offset);
            continue;
        }

        else if (esc_values[i] == 39) {
            current_primary = (size_t)-1;

            if (reverse_video) {
                term_swap_palette();
            }

            set_text_fg_default();

            if (reverse_video) {
                term_swap_palette();
            }

            continue;
        }

        else if (esc_values[i] == 49) {
            if (reverse_video) {
                term_swap_palette();
            }

            set_text_bg_default();

            if (reverse_video) {
                term_swap_palette();
            }

            continue;
        }

        else if (esc_values[i] == 7) {
            if (!reverse_video) {
                reverse_video = true;
                term_swap_palette();
            }
            continue;
        }

        else if (esc_values[i] == 27) {
            if (reverse_video) {
                reverse_video = false;
                term_swap_palette();
            }
            continue;
        }
    }
}

static void dec_private_parse(uint8_t c) {
    dec_private = false;

    if (esc_values_i == 0) {
        return;
    }

    bool set;

    switch (c) {
        case 'h':
            set = true; break;
        case 'l':
            set = false; break;
        default:
            return;
    }

    switch (esc_values[0]) {
        case 25: {
            if (set) {
                enable_cursor();
            } else {
                disable_cursor();
            }
            return;
        }
    }

    if (term_callback != NULL) {
        if (term_arg != 0) {
            term_callback(term_arg, TERM_CB_DEC, esc_values_i, (uintptr_t)esc_values, c);
        } else {
            term_callback(TERM_CB_DEC, esc_values_i, (uintptr_t)esc_values, c, 0);
        }
    }
}

static void linux_private_parse(void) {
    if (esc_values_i == 0) {
        return;
    }

    if (term_callback != NULL) {
        if (term_arg != 0) {
            term_callback(term_arg, TERM_CB_LINUX, esc_values_i, (uintptr_t)esc_values, 0);
        } else {
            term_callback(TERM_CB_LINUX, esc_values_i, (uintptr_t)esc_values, 0, 0);
        }
    }
}

static void mode_toggle(uint8_t c) {
    if (esc_values_i == 0) {
        return;
    }

    bool set;

    switch (c) {
        case 'h':
            set = true; break;
        case 'l':
            set = false; break;
        default:
            return;
    }

    switch (esc_values[0]) {
        case 4:
            insert_mode = set; return;
    }

    if (term_callback != NULL) {
        if (term_arg != 0) {
            term_callback(term_arg, TERM_CB_MODE, esc_values_i, (uintptr_t)esc_values, c);
        } else {
            term_callback(TERM_CB_MODE, esc_values_i, (uintptr_t)esc_values, c, 0);
        }
    }
}

static void control_sequence_parse(uint8_t c) {
    if (escape_offset == 2) {
        switch (c) {
            case '[':
                discard_next = true;
                goto cleanup;
            case '?':
                dec_private = true;
                return;
        }
    }

    if (c >= '0' && c <= '9') {
        if (esc_values_i == MAX_ESC_VALUES) {
            return;
        }
        rrr = true;
        esc_values[esc_values_i] *= 10;
        esc_values[esc_values_i] += c - '0';
        return;
    }

    if (rrr == true) {
        esc_values_i++;
        rrr = false;
        if (c == ';')
            return;
    } else if (c == ';') {
        if (esc_values_i == MAX_ESC_VALUES) {
            return;
        }
        esc_values[esc_values_i] = 0;
        esc_values_i++;
        return;
    }

    size_t esc_default;
    switch (c) {
        case 'J': case 'K': case 'q':
            esc_default = 0; break;
        default:
            esc_default = 1; break;
    }

    for (size_t i = esc_values_i; i < MAX_ESC_VALUES; i++) {
        esc_values[i] = esc_default;
    }

    if (dec_private == true) {
        dec_private_parse(c);
        goto cleanup;
    }

    bool r = scroll_disable();
    size_t x, y;
    get_cursor_pos(&x, &y);

    switch (c) {
        case 'F':
            x = 0;
            // FALLTHRU
        case 'A': {
            if (esc_values[0] > y)
                esc_values[0] = y;
            size_t orig_y = y;
            size_t dest_y = y - esc_values[0];
            bool will_be_in_scroll_region = false;
            if ((scroll_top_margin >= dest_y && scroll_top_margin <= orig_y)
             || (scroll_bottom_margin >= dest_y && scroll_bottom_margin <= orig_y)) {
                will_be_in_scroll_region = true;
            }
            if (will_be_in_scroll_region && dest_y < scroll_top_margin) {
                dest_y = scroll_top_margin;
            }
            set_cursor_pos(x, dest_y);
            break;
        }
        case 'E':
            x = 0;
            // FALLTHRU
        case 'e':
        case 'B': {
            if (y + esc_values[0] > term_rows - 1)
                esc_values[0] = (term_rows - 1) - y;
            size_t orig_y = y;
            size_t dest_y = y + esc_values[0];
            bool will_be_in_scroll_region = false;
            if ((scroll_top_margin >= orig_y && scroll_top_margin <= dest_y)
             || (scroll_bottom_margin >= orig_y && scroll_bottom_margin <= dest_y)) {
                will_be_in_scroll_region = true;
            }
            if (will_be_in_scroll_region && dest_y >= scroll_bottom_margin) {
                dest_y = scroll_bottom_margin - 1;
            }
            set_cursor_pos(x, dest_y);
            break;
        }
        case 'a':
        case 'C':
            if (x + esc_values[0] > term_cols - 1)
                esc_values[0] = (term_cols - 1) - x;
            set_cursor_pos(x + esc_values[0], y);
            break;
        case 'D':
            if (esc_values[0] > x)
                esc_values[0] = x;
            set_cursor_pos(x - esc_values[0], y);
            break;
        case 'c':
            if (term_callback != NULL) {
                if (term_arg != 0) {
                    term_callback(term_arg, TERM_CB_PRIVATE_ID, 0, 0, 0);
                } else {
                    term_callback(TERM_CB_PRIVATE_ID, 0, 0, 0, 0);
                }
            }
            break;
        case 'd':
            esc_values[0] -= 1;
            if (esc_values[0] >= term_rows)
                esc_values[0] = term_rows - 1;
            set_cursor_pos(x, esc_values[0]);
            break;
        case 'G':
        case '`':
            esc_values[0] -= 1;
            if (esc_values[0] >= term_cols)
                esc_values[0] = term_cols - 1;
            set_cursor_pos(esc_values[0], y);
            break;
        case 'H':
        case 'f':
            esc_values[0] -= 1;
            esc_values[1] -= 1;
            if (esc_values[1] >= term_cols)
                esc_values[1] = term_cols - 1;
            if (esc_values[0] >= term_rows)
                esc_values[0] = term_rows - 1;
            set_cursor_pos(esc_values[1], esc_values[0]);
            break;
        case 'n':
            switch (esc_values[0]) {
                case 5:
                    if (term_callback != NULL) {
                        if (term_arg != 0) {
                            term_callback(term_arg, TERM_CB_STATUS_REPORT, 0, 0, 0);
                        } else {
                            term_callback(TERM_CB_STATUS_REPORT, 0, 0, 0, 0);
                        }
                    }
                    break;
                case 6:
                    if (term_callback != NULL) {
                        if (term_arg != 0) {
                            term_callback(term_arg, TERM_CB_POS_REPORT, x + 1, y + 1, 0);
                        } else {
                            term_callback(TERM_CB_POS_REPORT, x + 1, y + 1, 0, 0);
                        }
                    }
                    break;
            }
            break;
        case 'q':
            if (term_callback != NULL) {
                if (term_arg != 0) {
                    term_callback(term_arg, TERM_CB_KBD_LEDS, esc_values[0], 0, 0);
                } else {
                    term_callback(TERM_CB_KBD_LEDS, esc_values[0], 0, 0, 0);
                }
            }
            break;
        case 'J':
            switch (esc_values[0]) {
                case 0: {
                    size_t rows_remaining = term_rows - (y + 1);
                    size_t cols_diff = term_cols - (x + 1);
                    size_t to_clear = rows_remaining * term_cols + cols_diff;
                    for (size_t i = 0; i < to_clear; i++) {
                        raw_putchar(' ');
                    }
                    set_cursor_pos(x, y);
                    break;
                }
                case 1: {
                    set_cursor_pos(0, 0);
                    bool b = false;
                    for (size_t yc = 0; yc < term_rows; yc++) {
                        for (size_t xc = 0; xc < term_cols; xc++) {
                            raw_putchar(' ');
                            if (xc == x && yc == y) {
                                set_cursor_pos(x, y);
                                b = true;
                                break;
                            }
                        }
                        if (b == true)
                            break;
                    }
                    break;
                }
                case 2:
                case 3:
                    clear(false);
                    break;
            }
            break;
        case '@':
            for (size_t i = term_cols - 1; ; i--) {
                term_move_character(i + esc_values[0], y, i, y);
                set_cursor_pos(i, y);
                raw_putchar(' ');
                if (i == x) {
                    break;
                }
            }
            set_cursor_pos(x, y);
            break;
        case 'P':
            for (size_t i = x + esc_values[0]; i < term_cols; i++)
                term_move_character(i - esc_values[0], y, i, y);
            set_cursor_pos(term_cols - esc_values[0], y);
            // FALLTHRU
        case 'X':
            for (size_t i = 0; i < esc_values[0]; i++)
                raw_putchar(' ');
            set_cursor_pos(x, y);
            break;
        case 'm':
            sgr();
            break;
        case 's':
            get_cursor_pos(&saved_cursor_x, &saved_cursor_y);
            break;
        case 'u':
            set_cursor_pos(saved_cursor_x, saved_cursor_y);
            break;
        case 'K':
            switch (esc_values[0]) {
                case 0: {
                    for (size_t i = x; i < term_cols; i++)
                        raw_putchar(' ');
                    set_cursor_pos(x, y);
                    break;
                }
                case 1: {
                    set_cursor_pos(0, y);
                    for (size_t i = 0; i < x; i++)
                        raw_putchar(' ');
                    break;
                }
                case 2: {
                    set_cursor_pos(0, y);
                    for (size_t i = 0; i < term_cols; i++)
                        raw_putchar(' ');
                    set_cursor_pos(x, y);
                    break;
                }
            }
            break;
        case 'r':
            scroll_top_margin = 0;
            scroll_bottom_margin = term_rows;
            if (esc_values_i > 0) {
                scroll_top_margin = esc_values[0] - 1;
            }
            if (esc_values_i > 1) {
                scroll_bottom_margin = esc_values[1];
            }
            if (scroll_top_margin >= term_rows
             || scroll_bottom_margin > term_rows
             || scroll_top_margin >= (scroll_bottom_margin - 1)) {
                scroll_top_margin = 0;
                scroll_bottom_margin = term_rows;
            }
            set_cursor_pos(0, 0);
            break;
        case 'l':
        case 'h':
            mode_toggle(c);
            break;
        case ']':
            linux_private_parse();
            break;
    }

    if (r)
        scroll_enable();

cleanup:
    control_sequence = false;
    escape = false;
}

static void restore_state(void) {
    bold = saved_state_bold;
    reverse_video = saved_state_reverse_video;
    current_charset = saved_state_current_charset;
    current_primary = saved_state_current_primary;

    term_restore_state();
}

static void save_state(void) {
    term_save_state();

    saved_state_bold = bold;
    saved_state_reverse_video = reverse_video;
    saved_state_current_charset = current_charset;
    saved_state_current_primary = current_primary;
}

static void escape_parse(uint8_t c) {
    escape_offset++;

    if (control_sequence == true) {
        control_sequence_parse(c);
        return;
    }

    if (csi == true) {
        csi = false;
        goto is_csi;
    }

    size_t x, y;
    get_cursor_pos(&x, &y);

    switch (c) {
        case '[':
is_csi:
            for (size_t i = 0; i < MAX_ESC_VALUES; i++)
                esc_values[i] = 0;
            esc_values_i = 0;
            rrr = false;
            control_sequence = true;
            return;
        case '7':
            save_state();
            break;
        case '8':
            restore_state();
            break;
        case 'c':
            term_reinit();
            clear(true);
            break;
        case 'D':
            if (y == scroll_bottom_margin - 1) {
                term_scroll();
                set_cursor_pos(x, y);
            } else {
                set_cursor_pos(x, y + 1);
            }
            break;
        case 'E':
            if (y == scroll_bottom_margin - 1) {
                term_scroll();
                set_cursor_pos(0, y);
            } else {
                set_cursor_pos(0, y + 1);
            }
            break;
        case 'M':
            // "Reverse linefeed"
            if (y == scroll_top_margin) {
                term_revscroll();
                set_cursor_pos(0, y);
            } else {
                set_cursor_pos(0, y - 1);
            }
            break;
        case 'Z':
            if (term_callback != NULL) {
                if (term_arg != 0) {
                    term_callback(term_arg, TERM_CB_PRIVATE_ID, 0, 0, 0);
                } else {
                    term_callback(TERM_CB_PRIVATE_ID, 0, 0, 0, 0);
                }
            }
            break;
        case '(':
        case ')':
            g_select = c - '\'';
            break;
        case '\e':
            if (term_runtime == false) {
                raw_putchar(c);
            }
            break;
    }

    escape = false;
}

static uint8_t dec_special_to_cp437(uint8_t c) {
    switch (c) {
        case '`': return 0x04;
        case '0': return 0xdb;
        case '-': return 0x18;
        case ',': return 0x1b;
        case '.': return 0x19;
        case 'a': return 0xb1;
        case 'f': return 0xf8;
        case 'g': return 0xf1;
        case 'h': return 0xb0;
        case 'j': return 0xd9;
        case 'k': return 0xbf;
        case 'l': return 0xda;
        case 'm': return 0xc0;
        case 'n': return 0xc5;
        case 'q': return 0xc4;
        case 's': return 0x5f;
        case 't': return 0xc3;
        case 'u': return 0xb4;
        case 'v': return 0xc1;
        case 'w': return 0xc2;
        case 'x': return 0xb3;
        case 'y': return 0xf3;
        case 'z': return 0xf2;
        case '~': return 0xfa;
        case '_': return 0xff;
        case '+': return 0x1a;
        case '{': return 0xe3;
        case '}': return 0x9c;
    }

    return c;
}

void term_putchar(uint8_t c) {
    if (discard_next || (term_runtime == true && (c == 0x18 || c == 0x1a))) {
        discard_next = false;
        escape = false;
        csi = false;
        control_sequence = false;
        g_select = 0;
        return;
    }

    if (escape == true) {
        escape_parse(c);
        return;
    }

    if (g_select) {
        g_select--;
        switch (c) {
            case 'B':
                charsets[g_select] = CHARSET_DEFAULT; break;
            case '0':
                charsets[g_select] = CHARSET_DEC_SPECIAL; break;
        }
        g_select = 0;
        return;
    }

    size_t x, y;
    get_cursor_pos(&x, &y);

    switch (c) {
        case 0x00:
        case 0x7f:
            return;
        case 0x9b:
            csi = true;
            // FALLTHRU
        case '\e':
            escape_offset = 0;
            escape = true;
            return;
        case '\t':
            if ((x / TERM_TABSIZE + 1) >= term_cols) {
                set_cursor_pos(term_cols - 1, y);
                return;
            }
            set_cursor_pos((x / TERM_TABSIZE + 1) * TERM_TABSIZE, y);
            return;
        case 0x0b:
        case 0x0c:
        case '\n':
            if (y == scroll_bottom_margin - 1) {
                term_scroll();
                set_cursor_pos(0, y);
            } else {
                set_cursor_pos(0, y + 1);
            }
            return;
        case '\b':
            set_cursor_pos(x - 1, y);
            return;
        case '\r':
            set_cursor_pos(0, y);
            return;
        case '\a':
            // The bell is handled by the kernel
            if (term_callback != NULL) {
                if (term_arg != 0) {
                    term_callback(term_arg, TERM_CB_BELL, 0, 0, 0);
                } else {
                    term_callback(TERM_CB_BELL, 0, 0, 0, 0);
                }
            }
            return;
        case 14:
            // Move to G1 set
            current_charset = 1;
            return;
        case 15:
            // Move to G0 set
            current_charset = 0;
            return;
    }

    if (insert_mode == true) {
        for (size_t i = term_cols - 1; ; i--) {
            term_move_character(i + 1, y, i, y);
            if (i == x) {
                break;
            }
        }
    }

    // Translate character set
    switch (charsets[current_charset]) {
        case CHARSET_DEFAULT:
            break;
        case CHARSET_DEC_SPECIAL:
            c = dec_special_to_cp437(c);
    }

    raw_putchar(c);
}