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
	Task first_task = tasks[current_task_idx];
    first_task.registers.rsp = (uint64_t)first_task.ustack;
    first_task.registers.ip = (uint64_t)first_task.entrypoint;

    // Switch to the first task's page map and enter userspace
	arch_scheduler_callback(NULL);
    arch_switch_pagemap(first_task);
    arch_enter_userspace((void *)first_task.entrypoint, (void *)first_task.ustack);

	// arch_switch_pagemap(tasks[current_task_idx]);
	// arch_enter_userspace((void *)tasks[current_task_idx].entrypoint, tasks[current_task_idx].ustack);
}

void sched_register_task(Task task)
{
	if (registered_tasks == 10)
		return;

	tasks[registered_tasks++] = task;
}

void print_registers(struct arch_task_registers* regs)
{
    debug(false, "Registers:\n");
    debug(false, "RAX: %p, RBX: %p, RCX: %p, RDX: %p\n", regs->registers.rax, regs->registers.rbx, regs->registers.rcx, regs->registers.rdx);
    debug(false, "RSI: %p, RDI: %p, RSP: %p, RBP: %p\n", regs->registers.rsi, regs->registers.rdi, regs->rsp, regs->registers.rbp);
    debug(false, "R8: %p, R9: %p, R10: %p, R11: %p\n", regs->registers.r8, regs->registers.r9, regs->registers.r10, regs->registers.r11);
    debug(false, "R12: %p, R13: %p, R14: %p, R15: %p\n", regs->registers.r12, regs->registers.r13, regs->registers.r14, regs->registers.r15);
    debug(false, "IP: %p\n", regs->ip);
}

void sched_reschedule(struct arch_task_registers* regs)
{
    // Save register state
    // regs->ip = tasks[current_task_idx].entrypoint;
    tasks[current_task_idx].registers = *regs;
    
    // Print old task registers
    // debug(true,"Old task registers:\n", tasks[current_task_idx].pagemap);
    // print_registers(&tasks[current_task_idx].registers);

    // Find new task
    if (registered_tasks > 1)
    {
        if (++current_task_idx >= registered_tasks)
            current_task_idx = 0;
    }
    else
        return;

    // Load new task
    auto new = tasks[current_task_idx];
    arch_switch_pagemap(new);
    
    // Restore context
    regs->ip = new.registers.ip;
    regs->registers = new.registers.registers;
    regs->rsp = new.registers.rsp;

    // Print new task registers
    debug(true,"New task registers:\n");
    print_registers(regs);

    debug(false, "new task: %s | %lx | %lx | %lx\n", new.descriptor, new.pagemap, new.entrypoint, new.registers.ip);
}

string_view get_current_task(void) {
	return tasks[current_task_idx].descriptor;
}