#define PR_MODULE "main"

#include <cpu.h>
#include <ipc/ipc.h>
#include <ipc/port.h>
#include <moon-sys/handover.h>
#include <moon-sys/time/sleep.h>
#include <platform.h>
#include <sched/scheduler.h>
#include <service/execution-space/create.h>
#include <sys/syscall.h>

#include "panic.h"
#include "printk.h"

NORETURN void kern_main(HandoverModules mods)
{
    panic_if(
        mods.count != 2,
        "Expected two handover modules (init, ramdisk). Received %d instead.",
        mods.count);

    auto initModule    = mods.modules[0];
    auto ramdiskModule = mods.modules[1];

    panic_if(strncmp(initModule.cmdline, "init.elf", 8) != 0,
             "First handover module is not init process");
    panic_if(strncmp(ramdiskModule.cmdline, "ramdisk", 7) != 0,
             "Second handover module is not ramdisk");

    arch_init_syscall();
    sched_prepare();
    ipc_init();

#if !defined(__x86_64__)
    panic("Platform not fully supported");
#endif

    auto space = UNWRAP(create_execution_space(initModule.address, 0));
    panic_if(ipc_assign_port(&space, PORT_INIT) == false,
             "Unable to assign IPC port to init process");
    sched_enqueue(space);

    sched_begin_work();
    arch_halt_cpu();
}
