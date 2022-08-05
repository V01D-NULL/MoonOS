#define PR_MODULE "main"

#include <stivale2.h>
#include <boot/boot.h>

#include <cpu.h>
#include <mm/dynamic/kmalloc.h>

#include <time/sleep.h>
#include <platform/acpi/x86/acpi.h>

#include <proc/sched/scheduler.h>
#include <proc/daemon/load.h>
#include <proc/uspace/syscalls.h>

#include "panic.h"
#include "printk.h"

#include <moon-extra/try-catch.h>
#include <moon-extra/result.h>

CAN_THROW
void test_try_throw(void)
{
	TRY_THROW_ENABLE_ALTERNATIVE(error);
	TRY(true);
	THROW("dummy throw");

	return;

error:
	trace("%s", throw_reason.reason);
	for (;;)
		;
}

Result test_result_err()
{
	return Error();
}

Result test_result_ok()
{
	return Okay(10);
}

#include <devices/term/tty.h>
void kern_main(BootContext *bootvars, struct stivale2_struct_tag_modules *mods)
{
	kmalloc_init();
	acpi_init();
	sleep_init();

	init_percpu(bootvars->rbp);
	init_syscalls();

	trace("Detected %d modules\n", mods->module_count);
	trace("Module string: %s\n", mods->modules[0].string);

	auto a = test_result_err();
	auto b = test_result_ok();
	trace("A: success: %ld\n", a.success);
	trace("B: success: %ld\n", b.success);
	
	// trace("result: %ld\n", UNWRAP_RESULT(a));
	UNWRAP_RESULT(b);

	test_try_throw();
	// load_daemon((const uint8_t *)mods->modules[0].begin, mods->modules[0].string);
	// sched_init();

	for (;;)
	{
		asm("hlt");
	}
}
