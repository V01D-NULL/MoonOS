#ifndef TIMER_H
#define TIMER_H

#include "../arch/x86/cpu/interrupts/interrupts.h"
#include "../drivers/screen/monitor.h"

void install_pit();
void sleep(u32int ms);

#endif //TIMER_H