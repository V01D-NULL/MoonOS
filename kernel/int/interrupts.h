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

struct iframe {
    gp_registers_t gpr;
    int64_t isr_number;
    int64_t error_code;
    int64_t rip;
    int64_t cs; 
    int64_t rflags; 
    int64_t rsp;
    int64_t ss;
};

static inline bool IS_IRQ(int vector) { return (vector <= IRQ15 && vector >= IRQ0); }

#define TRY_EXEC_HANDLER(base)                                                                     \
    {                                                                                              \
        if (isr_handler_array[regs.isr_number] != 0)                                               \
        {                                                                                          \
            isr_handler_array[regs.isr_number] GENERIC_CAST(struct iframe *, &regs);               \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            if (!IS_IRQ(regs.isr_number))                                                          \
            {                                                                                      \
                debug(true, "Unhandled interrupt 0x%x (%ld)\n", regs.isr_number, regs.isr_number); \
            }                                                                                      \
        }                                                                                          \
    }

typedef void (*isr_t)(struct iframe *);

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