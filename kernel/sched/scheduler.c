#define PR_MODULE "sched"

#include "scheduler.h"
#include <base/base-types.h>
#include <mm/virt.h>
#include <moon-ds/bitmap.h>
#include <moon-ds/containers.h>
#include <moon-extra/result.h>
#include <moon-io/serial.h>
#include <moon-sys/spinlock.h>
#include <printk.h>
#include <sys/context_switch.h>
#include <uspace/userspace.h>

long *pid_bitmap;
int   next_available_pid = 0;

cc_omap(int, ExecutionSpace) processes;
cc_vec(int) in_use_pids;
static int current_pid_index = 0;

void sched_prepare(void)
{
    init(&in_use_pids);
    init(&processes);
    pid_bitmap = calloc(MAX_PIDS * sizeof(long));
}

void sched_begin_work(void)
{
    trace(TRACE_MISC, "Starting scheduler\n");
    ExecutionSpace *es = get(&processes, 0);

    arch_switch_pagemap(es->vm_space);
    arch_start_scheduler_timer();
    arch_enter_userspace(
        (void *)es->ec.entry, (void *)es->stack_pointer, es->argc, es->argv);
}

void sched_enqueue(ExecutionSpace es)
{
    insert(&processes, es.pid, es);
}

SchedulerResult sched_reschedule(struct arch_task_registers *regs)
{
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
    if (unlikely(pid < 0 || pid >= size(&processes)))
        return NULL;

    return get(&processes, pid);
}

int sched_allocate_pid(void)
{
    if (next_available_pid >= MAX_PIDS)
        panic("Unable to allocate new PID because MAX_PIDS (%d) was reached",
              MAX_PIDS);

    __set_bit(pid_bitmap, next_available_pid);
    push(&in_use_pids, next_available_pid);
    return next_available_pid++;
}