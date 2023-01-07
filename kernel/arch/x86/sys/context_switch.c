#include "context_switch.h"
#include <interrupts.h>
// #include <moon-sys/spinlock.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>
#include <sched/scheduler.h>
#include <uspace/userspace.h>

// typedef void (*schedulerCallback)(struct arch_task_registers);
// extern volatile schedulerCallback sched_reschedule;
static long timer_quantum;

// create_lock("sched_lock", sched_lock);
void timer_irq(struct iframe *frame);


void arch_scheduler_callback(void *sched_handler_fn)
{
	// if (!is_isr_registered(IRQ0))
	// 	install_isr(IRQ0, (isr_t)&timer_irq);

	trace("%p\n", sched_reschedule);
	// sched_callback = (schedulerCallback)sched_handler_fn;
	timer_quantum = lapic_calibrate_timer(2000); // Timer triggers IRQ0 after 20 usec
	lapic_oneshot_timer(timer_quantum);
}

void timer_irq(struct iframe *frame)
{
	trace("yes\n");
	// clang-format off
	struct arch_task_registers regs = {
		.registers = frame->gpr,
		.ip = frame->rip
	};
	// clang-format on
	trace("yes part 2\n");
	
	trace("%p\n", sched_reschedule);
	// for(;;)
	// 	;
	__auto_type task = sched_reschedule(regs);
	trace("yes part 3\n");
	lapic_eoi();
	lapic_oneshot_timer(timer_quantum);
	trace("yes part 4\n");
	// arch_enter_userspace((void *)task.entrypoint, task.ustack);
	// for(;;)
	// 	;
}