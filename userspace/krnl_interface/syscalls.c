#include "syscalls.h"

// Todo....
// Also, take note of how this needs to be setup in order to port mlibc which should be done asap
void syscall(void)
{
    asm volatile(
        "syscall\n"
    );
}

// Note: The string should already be formatted when calling syscall_log()
// meaning it does NOT have printf() functionality.
void syscall_log(const char *msg)
{
    syscall();
}