#include "keyboard.h"
#include <int/isr_list.h>
#include <printk.h>
#include <drivers/io/serial.h>
#include <util/ptr.h>
void keyboard_handler(isr_t isr);

void generic_keyboard_init(charset_t charset)
{
    if (charset != CHARSET_EN_US)
        return;

    install_isr(IRQ1, (isr_t)&keyboard_handler);
    install_isr(ISR_VERBOSE_BOOT, (isr_t)&verbose_boot_irq);
}

void keyboard_handler(isr_t isr)
{
    uint8_t scancode = inb(0x60);

    /* Escape key - verbose boot isr */
    if (scancode == 0x1)
    {
        __asm__("int $48"); // Trigger verbose boot
    }
}