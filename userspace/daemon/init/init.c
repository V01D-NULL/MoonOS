#include <krnl_interface/syscalls.h>

void _start(void)
{
    syscall_log("init: Started init daemon\n", 27);
    for (;;) {}
}