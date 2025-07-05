#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

uint64_t syscall_log(const char *buff, size_t len);
uint64_t syscall_ipc_send(uint64_t to, uint64_t buff);
uint64_t syscall_ipc_receive(uint64_t buff);
uint64_t syscall_create_process(uint64_t elf, uint64_t port_id);

#endif  // SYSCALLS_H