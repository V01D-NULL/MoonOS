#ifndef GENERIC_KEYBOARD_H
#define GENERIC_KEYBOARD_H

#include <int/interrupts.h>

typedef enum {
    CHARSET_EN_US = 0
} charset_t;

void generic_keyboard_init(charset_t charset);

#endif // GENERIC_KEYBOARD_H