#include "syscalls.h"

// Take note of how this needs to be setup in order to port mlibc which should be done asap
//
void syscall(uint64_t selector, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
    /*
        Argument order/register set taken from linux for compatibility reasons.
        See: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86_64-64_bit
    */
    asm volatile(
        ".intel_syntax noprefix\n"
        
        "push rcx \n"
        "push r11 \n"
        
        /* There is no input constraint for any specific register ranging from (r8-r15) in gcc... */
        "mov r10, %4\n"
        "mov r8,  %5\n"
        "mov r9,  %6\n"
        "syscall    \n"
        "pop r11    \n"
        "pop rcx    \n"

        ".att_syntax prefix\n"
        :
        : "a"(selector), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3), "r"(arg4), "r"(arg5)
        : "memory", "r8", "r9", "r10"
    );
}

void syscall_log(const char *buff, size_t len)
{
    syscall(1, 0, (uint64_t)buff, len, 0, 0, 0);
}