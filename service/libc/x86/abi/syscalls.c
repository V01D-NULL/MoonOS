#include <abi/syscalls.h>
#include <stdint.h>

uint64_t syscall(uint64_t selector, uint64_t arg0, uint64_t arg1, uint64_t arg2,
                 uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
    /*
        Argument order/register set taken from linux for compatibility reasons.
        See:
       https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86_64-64_bit
    */
    uint64_t status;
    asm volatile(
        ".intel_syntax noprefix\n"

        "push rcx \n"
        "push r11 \n"

        /* There is no input constraint for any specific register ranging from
           (r8-r15) in gcc... */
        "mov r10, %4\n"
        "mov r8,  %5\n"
        "mov r9,  %6\n"
        "syscall    \n"
        "pop r11    \n"
        "pop rcx    \n"

        ".att_syntax prefix\n"
        : "=a"(status)
        : "a"(selector),
          "D"(arg0),
          "S"(arg1),
          "d"(arg2),
          "r"(arg3),
          "r"(arg4),
          "r"(arg5)
        : "memory", "r8", "r9", "r10");

    return status;
}

uint64_t syscall_log(const char *buff, size_t len)
{
    return syscall(1, 1, (uint64_t)buff, len, 0, 0, 0);
}

uint64_t syscall_ipc_send(uint64_t to, uint64_t buff)
{
    return syscall(2, to, buff, 0, 0, 0, 0);
}

uint64_t syscall_ipc_receive(uint64_t buff)
{
    return syscall(3, buff, 0, 0, 0, 0, 0);
}

uint64_t syscall_create_process(uint64_t elf, uint64_t port_id)
{
    return syscall(4, elf, port_id, 0, 0, 0, 0);
}