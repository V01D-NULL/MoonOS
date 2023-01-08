#ifndef TASK_H
#define TASK_H

#include <base/base-types.h>
#include <moon-extra/compiler.h>
#include <moon-sys/elf/elf64.h>
#include <paging/paging.h>
#include <sys/context_switch.h>
#include <cpu.h>

enum task_type
{
	/* Special microkernel service/daemon/server */
	TASK_DAEMON = 0,

	/* Regular user process */
	TASK_PROCESS = 1
};

struct$(Task, {
	string_view descriptor;
	struct Pml *pagemap;
	Elf64_Addr entrypoint;
	enum task_type task_type;
	size_t ustack;
	struct arch_task_registers registers;
});

Task create_task_struct(string_view descriptor, Elf64_Addr entrypoint);

#endif // TASK_H