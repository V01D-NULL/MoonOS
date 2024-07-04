#define PR_MODULE "daemon"

#include <loader/elf.h>
#include <panic.h>
#include <printk.h>
#include <sched/scheduler.h>
#include <uspace/userspace.h>

void load_daemon(const uint8_t *elf, string_view name)
{
    trace(TRACE_SERVICE, "Loading %s...\n", name);

    Task task = load_elf(elf, name, true);
    sched_register_task(task);

    if (!task.entrypoint)
        panic("Failed to load ELF; Cannot start kernel daemon!");

    trace(TRACE_SERVICE, "Successfully loaded %s daemon\n", name);
}