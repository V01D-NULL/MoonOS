#ifndef INTERRUPTS_H
#define INTERRUPTS_H


#include "../../../../common.h"
#include "../../../../hal/pic.h"

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

typedef struct registers
{
	u32int ds;                                      // Data segment selector
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
	u32int int_no, err_code;                        // Interrupt number and error code (if applicable)
	u32int eip, cs, eflags, useresp, ss;            // Pushed by the processor automatically.
}
registers_t;

typedef void ( *isr_t ) ( registers_t );            // isr_t is a pointer to a function that returns void and
                                                    // takes one argument of type registers_t

void register_interrupt_handler ( u8int n, isr_t handler );
void unregister_interrupt_hanlder(u8int n);

#endif  // INTERRUPTS_H
