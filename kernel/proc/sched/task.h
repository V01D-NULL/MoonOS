#ifndef TASK_H
#define TASK_H

#include <ktypes.h>
#include <mm/vmm.h>
#include <amd64/cpu.h>
#include <proc/elf/elf64.h>

enum task_type
{
	/* Special microkernel service/daemon/server */
	TASK_DAEMON = 0,

	/* Regular user process */
	TASK_PROCESS = 1
};

$struct(task_t, {
	string_view descriptor;
	struct Pml *pagemap;
	Elf64_Addr entrypoint;
	enum task_type task_type;
	size_t ustack;
});

task_t new_task(string_view descriptor, Elf64_Addr entrypoint);
void add_task(task_t task);

#endif // TASK_H