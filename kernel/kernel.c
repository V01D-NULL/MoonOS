#define PR_MODULE "main"

#include <moon-sys/handover.h>

#include <cpu.h>
#include <sys/syscall.h>

#include <moon-sys/time/sleep.h>

#include <platform.h>
#include <sched/scheduler.h>

#include "panic.h"
#include "printk.h"

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

    auto space = UNWRAP(
        create_execution_space((const uint8_t *)(mods.modules[0].address)));

    sched_enqueue(space.ec);
    sched_begin_work(space);

    arch_halt_cpu();
}
