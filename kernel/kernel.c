#define PR_MODULE "main"

#include <moon-sys/handover.h>

#if defined(__x86_64__)
#include <boot/stivale2.h>  // TEMPORARY (until stivale2_struct_tag_modules is replaced by HandoverModules)
#endif

#include <cpu.h>
#include <mm/dynamic/kmalloc.h>
#include <sys/syscall.h>

#include <moon-sys/time/sleep.h>

#include <loader/daemon/load.h>
#include <platform.h>
#include <sched/scheduler.h>

#include "panic.h"
#include "printk.h"

#if defined(__x86_64__)
void kern_main(HandoverModules mods)
#else
void kern_main(void)
#endif
{
    arch_init_syscall();

#if defined(__x86_64__)
    trace("Detected %d modules\n", mods.count);
    trace("Module string: %s\n", mods.modules[0].cmdline);

    load_daemon(
        (const uint8_t *)mods.modules[0].address, mods.modules[0].cmdline);
    load_daemon(
        (const uint8_t *)mods.modules[1].address, mods.modules[1].cmdline);

    sched_init();
#else
    panic("Modules have not been implemented yet");
#endif

    arch_halt_cpu();
}
