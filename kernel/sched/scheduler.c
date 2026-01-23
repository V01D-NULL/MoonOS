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

vec(ExecutionSpace) processes;
static int current_pid_index  = 0;
static int next_available_pid = 0;

void sched_prepare(void)
{
    init(&processes);
    next_available_pid = 0;
}

void sched_begin_work(void)
{
    trace(TRACE_MISC, "Starting scheduler\n");
    if (size(&processes) == 0)
        panic("No processes to schedule");

    ExecutionSpace *es = get(&processes, 0);

    arch_switch_pagemap(es->vm_space);
    arch_start_scheduler_timer();
    arch_enter_userspace(
        (void *)es->ec.entry, (void *)es->stack_pointer, es->argc, es->argv);
}

void sched_enqueue(ExecutionSpace es)
{
    push(&processes, es);
}

SchedulerResult sched_reschedule(struct arch_task_registers *regs)
{
    if (unlikely(processes == NULL || !size(&processes)))
        return Error(SchedulerResult, "No processes to schedule");

    // Save the current register state
    ExecutionSpace *current = get(&processes, current_pid_index);
    current->ec.registers   = *regs;

    // Select the next process
    current_pid_index   = (current_pid_index + 1) % size(&processes);
    ExecutionSpace next = *get(&processes, current_pid_index);

    return Okay(SchedulerResult, next);
}

ExecutionSpace *sched_current(void)
{
    if (unlikely(processes == NULL || !size(&processes)))
        return NULL;

    return get(&processes, current_pid_index);
}

ExecutionSpace *sched_get(int pid)
{
    for (size_t i = 0; i < size(&processes); i++)
    {
        ExecutionSpace *es = get(&processes, i);
        if (es && es->pid == pid)
            return es;
    }
    return NULL;
}

int sched_allocate_pid(void)
{
    if (next_available_pid >= MAX_PIDS)
        panic("Unable to allocate new PID because MAX_PIDS (%d) was reached",
              MAX_PIDS);
    return next_available_pid++;
}

int sched_dequeue(int pid)
{
    for (size_t i = 0; i < size(&processes); i++)
    {
        ExecutionSpace *es = get(&processes, i);
        if (es && es->pid == pid)
        {
            erase(&processes, i);
            return 0;
        }
    }
    return -1;
}