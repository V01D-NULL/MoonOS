#define PR_MODULE "main"

#include <moon-sys/handover.h>

#if defined (__x86_64__)
#include <boot/stivale2.h> // TEMPORARY (until stivale2_struct_tag_modules is replaced by HandoverModules)
#endif

#include <cpu.h>
#include <sys/syscall.h>
#include <mm/dynamic/kmalloc.h>

#include <moon-sys/time/sleep.h>

#include <sched/scheduler.h>
#include <loader/daemon/load.h>

#include "panic.h"
#include "printk.h"

#if defined(__x86_64__)
void kern_main(struct stivale2_struct_tag_modules *mods)
#else
void kern_main(void)
#endif
{
	arch_init_syscall();

#if defined(__x86_64__)
	trace("Detected %d modules\n", mods->module_count);
	trace("Module string: %s\n", mods->modules[0].string);

	load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);
	sched_init();
#endif

	for (;;)
	{
		asm("wfi");
	}
}
