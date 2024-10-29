#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <moon-extra/result.h>

typedef int FIXME;

typedef Result(FIXME, Nullish) SchedulerResult;

void        sched_init(void);
void        sched_register_task(FIXME task);
string_view get_current_task(void);

#endif  // SCHEDULER_H