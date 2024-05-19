#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void sched_init(void);
void sched_register_task(Task task);
void sched_yield(void);

string_view get_current_task(void);

#endif // SCHEDULER_H