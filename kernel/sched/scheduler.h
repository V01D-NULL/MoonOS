#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <moon-extra/result.h>
#include <service/execution-space/es.h>

typedef Result(ExecutionContext, Nullish) SchedulerResult;

void sched_prepare(void);
void sched_enqueue(ExecutionContext ec);
void sched_begin_work(ExecutionSpace es);

#endif  // SCHEDULER_H