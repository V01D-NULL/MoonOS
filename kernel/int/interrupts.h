#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "idt.h"

/*
    Example of registering a custom interrupt:

        void abc(regs_t r) {
            //do stuff
        }

        void some_func() {
            install_isr(255, &abc);
            asm("int $255");
        }
*/

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef void (*isr_t)(regs_t);

void install_isr(uint8_t base, isr_t handler);
void uninstall_isr(uint8_t base);

#endif // INTERRUPTS_H