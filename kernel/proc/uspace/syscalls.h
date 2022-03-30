#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <ktypes.h>

void init_syscalls(void);
bool fast_syscall_supported(void);

#endif // SYSCALLS_H