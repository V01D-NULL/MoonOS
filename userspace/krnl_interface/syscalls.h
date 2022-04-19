#ifndef USER_SYSCALLS_H
#define USER_SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

void syscall_log(const char *buff, size_t len);

#endif // USER_SYSCALLS_H