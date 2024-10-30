#include <abi/syscalls.h>

void main(void)
{
    syscall_log("init\n", 5);
    for (;;)
        ;
}