#define PR_MODULE "sched"

#include "scheduler.h"
#include <base/base-types.h>
#include <mm/virt.h>
#include <moon-ds/containers.h>
#include <moon-extra/result.h>
#include <moon-io/serial.h>
#include <moon-sys/spinlock.h>
#include <printk.h>
#include <sys/context_switch.h>
#include <uspace/userspace.h>

// TODO: Replace me with something like a map
cc_vec(ExecutionContext) processes;

// TODO: Remove me. This should not be necessary
static bool scheduler_running = false;

void sched_prepare(void)
{
    init(&processes);
}

void sched_begin_work(ExecutionSpace es)
{
    trace(TRACE_MISC, "Starting scheduler\n");

    scheduler_running = true;
    arch_switch_pagemap(es.vm_space);
    arch_enter_userspace(
        (void *)es.ec.entry, (void *)es.stack_pointer + PAGE_SIZE);
}

void sched_enqueue(ExecutionContext ec)
{
    push(&processes, ec);
}

SchedulerResult sched_reschedule(struct arch_task_registers *regs)
{
    debug(true, "Rescheduling\n");
    // if (unlikely(!scheduler_running))
    //     return Error(SchedulerResult, NULL);

    // // Save register state
    // tasks[current_task_idx].registers = *regs;

    // // Find new task
    // if (registered_tasks > 1)
    // {
    //     if (++current_task_idx >= registered_tasks)
    //         current_task_idx = 0;
    // }
    // else
    //     return;

    // // Load new task
    // auto new = tasks[current_task_idx];
    // // arch_switch_pagemap(new);

    // return Okay(SchedulerResult, new);
}
