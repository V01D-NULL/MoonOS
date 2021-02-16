#include "timer.h"

u32int tick = 0;

static void timer_handler_callback(registers_t regs)
{
    tick++;
}

void sleep(u32int ms)
{
    tick = 0; //Note: Changing the value of tick won't change the actual value of tick, it is unique to this scope
    while (tick <= ms)
    {
        asm volatile("nop");
    }
}

void install_pit()
{
    monitor_write("Installing PIT...\n", true, false);
    register_interrupt_handler(IRQ0, &timer_handler_callback);
}
