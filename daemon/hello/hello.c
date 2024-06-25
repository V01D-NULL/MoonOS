#include <abi/syscalls.h>

void main(void)
{
    for (;;)
        syscall_log("hello\n", 6);
}