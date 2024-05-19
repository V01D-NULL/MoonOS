#include "syscalls.h"
#include <moon-extra/compiler.h>
#include <base/string.h>
#include <cpu.h>
#include <printk.h>
#include <base/base-types.h>
#include <panic.h>
#include <moon-io/io.h>
#include <sched/scheduler.h>

#define Type uint64_t
#define ignore Type UNIQUE_NAME(ignore)

// Syscall stub
int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore) { return -1; }

int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
{
	(void)fd;

	if (buff == 0) return -1;

	string_view ptr = (string_view)buff;
	// debug(true, "len %d | strlen: %d\n", len, strlen(ptr));
	// if (strlen(ptr) == 0) {
	// 	debug(true, "oh fuck %s | %lx\n", ptr, buff);
	// }

	debug(true, "[%s]\n", get_current_task());
	// for (auto i = 0; i < len; i++)
	// 	puts("%c", ptr[i]);

	return 0;
}
