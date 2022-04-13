#include <krnl_interface/syscalls.h>

void _start(void)
{
    syscall_log("Hi");
    for (;;) {}
}