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
		.rsp = frame->rsp,
		.ip = frame->rip,
	};
	// clang-format on

	debug(true, "== timer irq ==\n");
	debug(false, "Register dump:\n"
					 "rax %p | rbx %p | rcx %p | rdx    %p\n"
					 "rbp %p | rsp %p | rdi %p | rsi    %p\n"
					 "rip %p | cs  %p | ss  %p | rflags %p\n"
					 "r8  %p | r9  %p | r10 %p | r11    %p\n"
					 "r12 %p | r13 %p | r14 %p | r15    %p\n",
			  frame->gpr.rax,
			  frame->gpr.rbx,
			  frame->gpr.rcx,
			  frame->gpr.rdx,
			  frame->gpr.rbp,
			  frame->rsp,
			  frame->gpr.rdi,
			  frame->gpr.rsi,
			  frame->rip,
			  frame->cs,
			  frame->ss,
			  frame->rflags,
			  frame->gpr.r8,
			  frame->gpr.r9,
			  frame->gpr.r10,
			  frame->gpr.r11,
			  frame->gpr.r12,
			  frame->gpr.r13,
			  frame->gpr.r14,
			  frame->gpr.r15);
			  

	sched_reschedule(&regs);
	
	lapic_eoi();
	lapic_oneshot_timer(timer_quantum);
}