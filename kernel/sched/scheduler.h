#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <moon-extra/result.h>
#include <service/execution-space/es.h>

typedef Result(ExecutionSpace, Nullish) SchedulerResult;

void            sched_prepare(void);
void            sched_enqueue(ExecutionSpace es);
void            sched_begin_work(void);
ExecutionSpace *sched_current(void);
ExecutionSpace *sched_get(int pid);

#endif  // SCHEDULER_H