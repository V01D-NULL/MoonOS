#include "mouse.h"

static void mouse_handler_callback(registers_t regs) {
    kprintf("Mouse IRQ fired\n");
}

void init_mouse()
{
    monitor_write("Installing mouse", true, false);
    register_interrupt_handler(IRQ12, mouse_handler_callback);
}