#define PR_MODULE "main"

#include <base/fmt.h>
#include <cpu.h>
#include <ipc/ipc.h>
#include <ipc/port.h>
#include <mm/phys.h>
#include <mm/virt.h>
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
    panic_if(mods.count != 2,
             "Expected 2 handover modules, received '%d'",
             mods.count);

    auto initModule = mods.modules[0];
    panic_if(strncmp(initModule.cmdline, "init.elf", 8) != 0,
             "First handover module is not init process");

    auto ramdiskModule = mods.modules[1];
    panic_if(strncmp(ramdiskModule.cmdline, "ramdisk", 7) != 0,
             "Second handover module is not ramdisk");

    arch_init_syscall();
    sched_prepare();
    ipc_init();

#if !defined(__x86_64__)
    panic("Platform not fully supported");
#endif

    ArgumentVector argv;
    init(&argv);
    push(&argv, "init.elf");

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "0x%p", pa(ramdiskModule.address));
    push(&argv, buffer);

    auto space = UNWRAP(create_execution_space(initModule.address, argv));
    cleanup(&argv);

    panic_if(ipc_assign_port(&space, PORT_INIT) == false,
             "Unable to assign IPC port to init process");
    sched_enqueue(space);

    Range range = {.base  = pa(ramdiskModule.address),
                   .limit = pa(ramdiskModule.address) + ramdiskModule.size};
    arch_map_range(space.vm_space, range, MAP_USER_RO, 0);

    sched_begin_work();
    arch_halt_cpu();
}
