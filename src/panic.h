#ifndef PANIC_H
#define PANIC_H

#include "arch/x86/cpu/interrupts/interrupts.h"

#define PANIC( b ) panic_assert( __FILE__, __LINE__, #b);
#define ASSERT( b ) ( ( b ) ? ( int ) 0 : assert( __FILE__, __LINE__, #b ) );

void dump_regs_interrupt(registers_t r);
void dump_regs(void);

extern void panic_assert ( const char *file, u32int line, const char *desc );
extern int assert ( const char *file, u32int line, const char *desc );

#endif