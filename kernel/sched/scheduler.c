#define PR_MODULE "sched"

#include "scheduler.h"
#include <moon-sys/spinlock.h>
#include <uspace/userspace.h>
#include <moon-io/serial.h>
#include <base/base-types.h>
#include <mm/virt.h>
#include <sys/context_switch.h>
#include <printk.h>

// Keep it simple and make it work for now
static Task tasks[10];
static int registered_tasks = 0, current_task_idx = 0;

void sched_init(void)
{
	arch_scheduler_callback(NULL);
	arch_switch_pagemap(tasks[current_task_idx]);
	arch_enter_userspace((void *)tasks[current_task_idx].entrypoint, tasks[current_task_idx].ustack);
}

void sched_register_task(Task task)
{
	if (registered_tasks == 10)
		return;

	tasks[registered_tasks++] = task;
}

// TODO: Add idle() task when no processes exist.
void sched_reschedule(struct arch_task_registers* regs)
{
	// Save register state
	tasks[current_task_idx].registers = *regs;
	// Find new task
	if (registered_tasks > 1)
	{
		if (++current_task_idx >= registered_tasks)
			current_task_idx = 0;
	}
	else
		return;


	// trace("%d %d\n", current_task_idx, registered_tasks);

	// Load new task
	auto new = tasks[current_task_idx];
	arch_switch_pagemap(new);
	regs->ip = new.registers.ip;
	// debug(false, "new task: %s\n", new.descriptor);
}