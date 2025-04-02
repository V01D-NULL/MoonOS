#define PR_MODULE "main"

#include <moon-sys/handover.h>

#include <cpu.h>
#include <sys/syscall.h>

#include <moon-sys/time/sleep.h>

#include <platform.h>
#include <sched/scheduler.h>

#include "panic.h"
#include "printk.h"

#include <moon-ds/containers.h>
#include <service/execution-space/create.h>

NORETURN void kern_main(HandoverModules mods)
{
    arch_init_syscall();
    sched_prepare();

#if !defined(__x86_64__)
    panic("Platform not fully supported");
#endif

    trace(TRACE_MISC, "Detected %d modules\n", mods.count);
    trace(TRACE_MISC, "Module string: %s\n", mods.modules[0].cmdline);

    for (int i = 0; i < mods.count; i++)
    {
        auto space = UNWRAP(create_execution_space(
            (const uint8_t *)(mods.modules[i].address), i));

        sched_enqueue(space);
    }

    sched_begin_work();
    arch_halt_cpu();
}
