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
#include <stdbool.h>
#include "isr_list.h"

static inline bool IS_IRQ(int vector) { return (vector <= IRQ15 && vector >= IRQ0); }

#define TRY_EXEC_HANDLER(base)  {                                                              \
    if (isr_handler_array[regs.isr_number] != 0)                                               \
    {                                                                                          \
        isr_handler_array[regs.isr_number] GENERIC_CAST(regs_t *, &regs);                      \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
        if (!IS_IRQ(regs.isr_number))                                                          \
        {                                                                                      \
            debug(true, "Unhandled interrupt 0x%x (%ld)\n", regs.isr_number, regs.isr_number); \
        }                                                                                      \
    }                                                                                          \
}

typedef void (*isr_t)(regs_t *);

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