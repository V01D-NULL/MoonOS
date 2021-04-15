/**
 * @file interrupts.h
 * @author Tim (V01D)
 * @brief Interrupt handler
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

typedef void (*isr_t)(regs_t*);

/**
 * @brief Register an ISR
 * 
 * @param base      The ISR's index in the IDT aka the interrupt number
 * @param handler   A function callback (the given function will be executed when the ISR is executed)
 */
void install_isr(uint8_t base, isr_t handler);

/**
 * @brief Remove an ISR from the IDT
 * 
 * @param base The ISR's index in the IDT aka the interrupt number
 */
void uninstall_isr(uint8_t base);

#endif // INTERRUPTS_H