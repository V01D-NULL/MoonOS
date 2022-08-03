#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "descriptors/idt.h"
#include <base/base-types.h>

enum
{
	IRQ0 = 32,
	IRQ1 = 33,
	IRQ2 = 34,
	IRQ3 = 35,
	IRQ4 = 36,
	IRQ5 = 37,
	IRQ6 = 38,
	IRQ7 = 39,
	IRQ8 = 40,
	IRQ9 = 41,
	IRQ10 = 42,
	IRQ11 = 43,
	IRQ12 = 44,
	IRQ13 = 45,
	IRQ14 = 46,
	IRQ15 = 47
};

struct iframe
{
	general_registers gpr;
	int64_t isr_number;
	int64_t error_code;
	int64_t rip;
	int64_t cs;
	int64_t rflags;
	int64_t rsp;
	int64_t ss;
};

#define IS_IRQ(vector) (vector <= IRQ15 && vector >= IRQ0)

#define TRY_EXEC_HANDLER(base)                                                                     \
	{                                                                                              \
		if (isr_handler_array[regs.isr_number] != 0)                                               \
		{                                                                                          \
			isr_handler_array[regs.isr_number]((struct iframe *)&regs);                            \
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

void install_isr(uint8_t base, isr_t handler);
void uninstall_isr(uint8_t base);

bool is_isr_registered(uint8_t isr);

#endif // INTERRUPTS_H