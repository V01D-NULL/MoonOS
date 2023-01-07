#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void sched_init(void);
void sched_register_task(Task task);
void sched_yield(void);
Task sched_reschedule(struct arch_task_registers regs);

#endif // SCHEDULER_H