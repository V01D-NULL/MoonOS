#include "context_switch.h"
#include <interrupts.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>
#include <sched/scheduler.h>

extern SchedulerResult sched_reschedule(struct arch_task_registers *regs);
static long            timer_quantum;

// create_lock("sched_lock", sched_lock);
static bool scheduler_enabled = false;

extern void task_switch_handler(struct iframe *frame);

void arch_scheduler_callback(void *sched_handler_fn)
{
    scheduler_enabled = true;
    timer_quantum =
        lapic_calibrate_timer(2000);  // Timer triggers IRQ0 after 20 usec
    lapic_oneshot_timer(timer_quantum);
}

void timer_irq(struct iframe *frame)
{
    // if (unlikely(!scheduler_enabled))
    //     return;

    // clang-format off
	struct arch_task_registers regs = {
		.registers = frame->gpr,
		.rsp = frame->rsp,
		.ip = frame->rip,
	};
    // clang-format on

    // debug(true, "== timer irq ==\n");
    // debug(false,
    //       "Register dump:\n"
    //       "rax %p | rbx %p | rcx %p | rdx    %p\n"
    //       "rbp %p | rsp %p | rdi %p | rsi    %p\n"
    //       "rip %p | cs  %p | ss  %p | rflags %p\n"
    //       "r8  %p | r9  %p | r10 %p | r11    %p\n"
    //       "r12 %p | r13 %p | r14 %p | r15    %p\n",
    //       frame->gpr.rax,
    //       frame->gpr.rbx,
    //       frame->gpr.rcx,
    //       frame->gpr.rdx,
    //       frame->gpr.rbp,
    //       frame->rsp,
    //       frame->gpr.rdi,
    //       frame->gpr.rsi,
    //       frame->rip,
    //       frame->cs,
    //       frame->ss,
    //       frame->rflags,
    //       frame->gpr.r8,
    //       frame->gpr.r9,
    //       frame->gpr.r10,
    //       frame->gpr.r11,
    //       frame->gpr.r12,
    //       frame->gpr.r13,
    //       frame->gpr.r14,
    //       frame->gpr.r15);

    // debug(true, "RIP BEFORE %p\n", frame->rip);
    auto res = sched_reschedule(&regs);
    if (!res.is_ok)
        return;

    // Task newTask = res.ok;
    // frame->rip   = newTask.registers.ip;
    // frame->gpr   = newTask.registers.registers;
    // frame->rsp   = newTask.registers.rsp;

    // debug(true, "RIP AFTER %p\n", frame->rip);

    lapic_eoi();
    lapic_oneshot_timer(timer_quantum);
}