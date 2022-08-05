#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <base/base-types.h>
#include <moon.h>

#define SyscallArgs uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t

int sys_stub(SyscallArgs);
int sys_log(SyscallArgs);

// clang-format off
USED
static typeof(int (*)(SyscallArgs))
syscall_list[] = {
	sys_stub,
	sys_log
};
// clang-format on

#endif // SYSCALLS_H