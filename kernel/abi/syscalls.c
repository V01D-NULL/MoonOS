#include "syscalls.h"
#include <moon-extra/compiler.h>
#include <cpu.h>
#include <printk.h>
#include <base/base-types.h>
#include <panic.h>

#define Type uint64_t
#define ignore Type UNIQUE_NAME(ignore)

// Syscall stub
int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore) { return -1; }

int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
{
	(void)fd;

	string_view ptr = (string_view)buff;
	if (!ptr)
		return -1;

	for (auto i = 0; i < len; i++)
		puts("%c", ptr[i]);

	return 0;
}
