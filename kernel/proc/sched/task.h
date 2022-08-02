#ifndef TASK_H
#define TASK_H

#include <ktypes.h>
#include <util/compiler.h>
#include <mm/vmm.h>
#include <amd64/cpu.h>
#include <proc/elf/elf64.h>
#include <int/interrupts.h>

enum task_type
{
	/* Special microkernel service/daemon/server */
	TASK_DAEMON = 0,

	/* Regular user process */
	TASK_PROCESS = 1
};

struct task_registers
{
	struct iframe general_purpose;

	// int64_t rip;
	// int64_t rflags;
	// int64_t cs;
};

// clang-format off
$struct(Task,
{
	string_view descriptor;
	struct Pml *pagemap;
	Elf64_Addr entrypoint;
	enum task_type task_type;
	size_t ustack;
	struct task_registers registers;
});
// clang-format on

Task create_task_struct(string_view descriptor, Elf64_Addr entrypoint);

#endif // TASK_H