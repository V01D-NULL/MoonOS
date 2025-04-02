#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

void syscall_log(const char *buff, size_t len);
void syscall_ipc_send(uint64_t to, uint64_t buff);
void syscall_ipc_receive(uint64_t buff);

#endif  // SYSCALLS_H