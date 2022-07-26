#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void sched_init(void);
void sched_register_task(task_t task);
void sched_yield(void);

#endif // SCHEDULER_H