#ifndef TTY_H
#define TTY_H

#include <base/fmt.h>
#include <base/string.h>

void arch_tty_init(uint64_t fb_addr, uint16_t fb_width, uint16_t fb_height,
                   uint16_t fb_pitch, uint8_t fb_red_mask_size,
                   uint8_t fb_red_mask_shift, uint8_t fb_green_mask_size,
                   uint8_t fb_green_mask_shift, uint8_t fb_blue_mask_size,
                   uint8_t fb_blue_mask_shift);
void arch_tty_write(string_view str);

#endif  // TTY_H