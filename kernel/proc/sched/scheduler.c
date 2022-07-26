#define PR_MODULE "sched"

#include "scheduler.h"
#include <proc/smp/spinlock.h>
#include <proc/uspace/userspace.h>
#include <int/interrupts.h>
#include <hal/apic/apic.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <ktypes.h>

// Follow KISS for now (Keep It Simple Stupid)
static task_t tasks[10];
static task_t *current_task;
static int counter = 0, current_task_idx = 0;
static int ticks_for_quantum = 0;

static lock_t sched_lock;

void sched_irq(struct iframe *isr)
{	
	debug(true, "sched_irq\n");
	// switch_to_kernel_pagemap();
	// auto new = tasks[0];
	// lapic_eoi();
	// sched_timer_oneshot();
	// switch_pagemap(new);
	// enter_ring3((void *)new.entrypoint, new.ustack);
	// debug(true, "TICK (cs: 0x%x)\n", isr->cs);
	sched_yield();
}

void sched_init(void)
{
	if (!is_isr_registered(IRQ0))
		install_isr(IRQ0, (isr_t)&sched_irq);

	ticks_for_quantum = lapic_calibrate_timer(20); // Timer triggers IRQ0 after 20 usec
	sched_timer_oneshot();
}

void sched_register_task(task_t task)
{
	if (counter == 10)
		return;

	tasks[counter++] = task;
}

// TODO: Save and restore register state for multiple processes
void sched_yield(void)
{
	acquire_lock(&sched_lock);
	// if (counter <= 1 && current_task != NULL)
	// 	return;

	switch_to_kernel_pagemap();

	// if (++current_task_idx > 10)
	// 	current_task_idx = 0;
	auto new = tasks[current_task_idx];
	// current_task = &new;
	
	// trace("sched_yield(): Giving task '%s' quantum\n", new.descriptor);
	// v_map_fast(new.pagemap, 0, 0, 7);
	// v_map_fast(new.pagemap, 0xfee00000, 0xfee00000, MAP_KERN);

	switch_pagemap(new);
	
	lapic_eoi();
    lapic_oneshot_timer(ticks_for_quantum);

	release_lock(&sched_lock);
    enter_ring3((void *)new.entrypoint, new.ustack);
}

void sched_timer_oneshot(void)
{
	lapic_oneshot_timer(ticks_for_quantum);
}