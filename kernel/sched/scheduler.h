#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <moon-extra/result.h>
#include "task.h"

typedef Result(Task, Nullish) SchedulerResult;

void        sched_init(void);
void        sched_register_task(Task task);
string_view get_current_task(void);

#endif  // SCHEDULER_H