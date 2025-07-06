#include <abi/syscalls.h>
#include <ipc/ipc.h>

void main(void)
{
    ipc_send(PORT_INIT, (uint64_t)"Hello", 5);

    for (;;)
        ;
}
