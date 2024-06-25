#include "tty.h"

#include <devices/term/early/early_term.h>
#include <devices/term/flanterm/backends/fb.h>
#include <devices/term/flanterm/flanterm.h>

static struct flanterm_context *ft_ctx;

void arch_tty_init(uint64_t fb_addr, uint16_t fb_width, uint16_t fb_height,
                   uint16_t fb_pitch, uint8_t fb_red_mask_size,
                   uint8_t fb_red_mask_shift, uint8_t fb_green_mask_size,
                   uint8_t fb_green_mask_shift, uint8_t fb_blue_mask_size,
                   uint8_t fb_blue_mask_shift)
{
    // clang-format off
    ft_ctx = flanterm_fb_init(
        NULL, NULL,
        fb_addr,
        fb_width,
        fb_height,
        fb_pitch,
        fb_red_mask_size,
        fb_red_mask_shift,
        fb_green_mask_size,
        fb_green_mask_shift,
        fb_blue_mask_size,
        fb_blue_mask_shift,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        0, 0, 1, 0, 0, 0
    );
    // clang-format on
}

void arch_tty_write(string_view str)
{
    // if (is_boot_term_available())
    //     boot_term_write("%s", str);
    // else
    flanterm_write(ft_ctx, str, strlen(str));
}
