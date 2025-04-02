#include <abi/syscalls.h>
#include <ipc/ipc.h>
#include <memory.h>

void main(void)
{
    for (;;)
    {
        struct IpcMessage *buffer = ipc_receive();
        if (buffer->message.type == IPC_MESSAGE_TYPE_RESPONSE)
        {
            syscall_log(&buffer->message.payload, buffer->message.payload_size);
            ipc_ack();
        }
    }
}
