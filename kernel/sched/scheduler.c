#define PR_MODULE "sched"

#include "scheduler.h"
#include <moon-sys/spinlock.h>
#include <uspace/userspace.h>
#include <interrupts.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>
#include <moon-io/serial.h>
#include <base/base-types.h>
#include <mm/virt.h>
#include <sys/context_switch.h>

// Keep it simple and make it work for now
static Task tasks[10];
static int registered_tasks = 0, current_task_idx = 0;
// static int ticks_for_quantum = 0;
// create_lock("sched_lock", sched_lock);

// void sched_timer_oneshot(void);
void sched_reschedule(struct arch_task_registers regs);

// void sched_irq(struct iframe *isr)
// {
// 	// debug(true, "sched_irq on rip: %lx\n", isr->rip);
// 	struct task_registers regs = {
// 		.general_purpose = {isr->gpr},
// 	};

// 	sched_reschedule(regs);
// }

void sched_init(void)
{
	arch_scheduler_callback((void*)&sched_reschedule);
	arch_switch_pagemap(tasks[current_task_idx]);
	arch_enter_userspace((void *)tasks[current_task_idx].entrypoint, tasks[current_task_idx].ustack);
}

void sched_register_task(Task task)
{
	if (registered_tasks == 10)
		return;

	tasks[registered_tasks++] = task;
}

// TODO: Save and restore register state for multiple processes
// TODO: Add idle() task when no processes exist.
void sched_reschedule(struct arch_task_registers regs)
{
	// acquire_lock(&sched_lock);

	// Find new task
	if (registered_tasks > 1)
	{
		if (++current_task_idx > 10)
			current_task_idx = 0;
	}
	else
		return; // goto reschedule;

	// Save register state
	tasks[current_task_idx].registers = regs;

	// Load new task
	auto new = tasks[current_task_idx];
	arch_switch_pagemap(new);

// reschedule:
	// return;
	// sched_timer_oneshot();
	// release_lock(&sched_lock);
}