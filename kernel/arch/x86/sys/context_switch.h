#ifndef CONTEXT_SWITCH_H
#define CONTEXT_SWITCH_H

#include <cpu.h>

struct arch_task_registers
{
	GeneralRegisters registers;
	int64_t rip;
};

void arch_scheduler_callback(void *sched_handler_fn);

#endif // CONTEXT_SWITCH_H