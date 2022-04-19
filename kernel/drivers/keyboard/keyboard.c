#include "keyboard.h"
#include <int/isr_list.h>
#include <printk.h>

void generic_keyboard_init(charset_t charset)
{
    if (charset != CHARSET_EN_US)
        return;

    // install_isr(IRQ1, (isr_t)&keyboard_handler);
}