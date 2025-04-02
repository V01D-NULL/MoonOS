#include <abi/syscalls.h>
#include <ipc/ipc.h>

void main(void)
{
    // uint8_t *buffer = ipc_get_buffer();
    // syscall_ipc_send(0, (uint64_t)buffer);

    ipc_send(0, (uint64_t) "Hello", 5);

    for (;;)
        ;
}
