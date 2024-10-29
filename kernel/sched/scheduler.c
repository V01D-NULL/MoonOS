#define PR_MODULE "sched"

#include "scheduler.h"
#include <base/base-types.h>
#include <mm/virt.h>
#include <moon-extra/result.h>
#include <moon-io/serial.h>
#include <moon-sys/spinlock.h>
#include <printk.h>
#include <sys/context_switch.h>
#include <uspace/userspace.h>

// Keep it simple and make it work for now
// static Task tasks[10];
static int  registered_tasks = 0, current_task_idx = 0;
static bool scheduler_running = false;

void sched_init(void)
{
    // Task first_task          = tasks[current_task_idx];
    // first_task.registers.rsp = (uint64_t)first_task.ustack;
    // first_task.registers.ip  = (uint64_t)first_task.entrypoint;

    // // Switch to the first task's page map and enter userspace
    // scheduler_running = true;
    // arch_scheduler_callback(NULL);
    // // arch_switch_pagemap(first_task);
    // arch_enter_userspace(
    //     (void *)first_task.entrypoint, (void *)first_task.ustack);
}

void sched_register_task(FIXME task)
{
    // if (registered_tasks == 10)
    //     return;

    // tasks[registered_tasks++] = task;
}

SchedulerResult sched_reschedule(struct arch_task_registers *regs)
{
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

string_view get_current_task(void)
{
    // return tasks[current_task_idx].descriptor;
}