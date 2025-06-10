#include <abi/syscalls.h>
#include <ipc/ipc.h>
#include <memory.h>

// TODO:
//  This init process will setup the system and it's services (kinda like
//  systemd) Steps:
//   For each system service (fs, net, etc):
//     1. Register a port for each process (init is the only process with
//     permissions to assign SYSTEM ports. Other processes can assign ports, but
//     none that are reserved for system use (example: net, fs))
//     2. Launch the service

void main(int argc, char **argv)
{
    // Test: first argument is "init.elf". Log it. (Second argument is ramdisk
    // with kernel services. Load those next..)
    if (argc == 2)
    {
        syscall_log(argv[0], 8);
    }

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
