#include "context_switch.h"
#include <interrupts.h>
// #include <moon-sys/spinlock.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>

extern void sched_reschedule(struct arch_task_registers *regs);
static long timer_quantum;

// create_lock("sched_lock", sched_lock);
void timer_irq(struct iframe *frame);

void arch_scheduler_callback(void *sched_handler_fn)
{
	if (!is_isr_registered(IRQ0))
		install_isr(IRQ0, (isr_t)&timer_irq);

	timer_quantum = lapic_calibrate_timer(2000); // Timer triggers IRQ0 after 20 usec
	lapic_oneshot_timer(timer_quantum);
}

void timer_irq(struct iframe *frame)
{
	// clang-format off
	struct arch_task_registers regs = {
		.registers = frame->gpr,
		.ip = frame->rip
	};
	// clang-format on

	// trace("rip: %p\n", frame->rip);
	sched_reschedule(&regs);
	frame->rip = regs.ip;
	// trace("rip: %p\n", frame->rip);
	lapic_eoi();
	lapic_oneshot_timer(timer_quantum);
}