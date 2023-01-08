#define PR_MODULE "daemon"

#include <sched/scheduler.h>
#include <uspace/userspace.h>
#include <loader/elf.h>
#include <printk.h>
#include <panic.h>

void load_daemon(const uint8_t *elf, string_view name)
{
	trace("Loading %s...\n", name);

	Task task = load_elf(elf, name, true);
	sched_register_task(task);

	if (!task.entrypoint)
		panic("Failed to load ELF; Cannot start kernel daemon!");

	trace("Successfully loaded %s daemon\n", name);
}