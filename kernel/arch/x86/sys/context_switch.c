#include "context_switch.h"
#include <interrupts.h>
#include <mm/virt.h>
#include <platform/acpi/x86/apic/apic.h>
#include <printk.h>
#include <sched/scheduler.h>

extern SchedulerResult sched_reschedule(struct arch_task_registers *regs);
static long            timer_quantum;

void arch_start_scheduler_timer(void)
{
    timer_quantum =
        lapic_calibrate_timer(2000);  // Timer triggers IRQ0 after 20 usec
    lapic_oneshot_timer(timer_quantum);
}

void arch_reschedule_now(void)
{
    lapic_oneshot_timer(1);
}

void timer_irq(struct iframe *frame)
{
    struct arch_task_registers regs = {
        .registers = frame->gpr,
        .rsp       = frame->rsp,
        .ip        = frame->rip,
    };

    ExecutionSpace newSpace = UNWRAP(sched_reschedule(&regs));

    frame->rip = newSpace.ec.registers.ip;
    frame->gpr = newSpace.ec.registers.registers;
    frame->rsp = newSpace.ec.registers.rsp;

    arch_switch_pagemap(newSpace.vm_space);

    lapic_eoi();
    lapic_oneshot_timer(timer_quantum);
}