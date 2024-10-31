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

cc_vec(ExecutionSpace) processes;

void sched_prepare(void)
{
    init(&processes);
}

void sched_begin_work(void)
{
    trace(TRACE_MISC, "Starting scheduler\n");

    ExecutionSpace *es = get(&processes, 0);

    arch_switch_pagemap(es->vm_space);
    arch_start_scheduler_timer();
    arch_enter_userspace((void *)es->ec.entry, (void *)es->stack_pointer);
}

void sched_enqueue(ExecutionSpace es)
{
    push(&processes, es);
}

SchedulerResult sched_reschedule(struct arch_task_registers *regs)
{
    static size_t current_index = 0;

    // Save the current register state
    ExecutionSpace *current = get(&processes, current_index);
    current->ec.registers   = *regs;

    // Select the next process
    current_index       = (current_index + 1) % size(&processes);
    ExecutionSpace next = *get(&processes, current_index);

    return Okay(SchedulerResult, next);
}
