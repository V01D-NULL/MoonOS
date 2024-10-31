#ifndef CONTEXT_SWITCH_H
#define CONTEXT_SWITCH_H

#include <cpu.h>

struct arch_task_registers
{
    GeneralRegisters registers;
    int64_t          ip;
    int64_t          rsp;
};

void arch_start_scheduler_timer(void);

#endif  // CONTEXT_SWITCH_H