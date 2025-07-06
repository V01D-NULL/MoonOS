#include <abi/syscalls.h>
#include <base-types.h>
#include <ipc/ipc.h>
#include <memory.h>
#include <string.h>
#include "utils/tar.h"

// TODO:
//  This init process will setup the system and it's services (kinda like
//  systemd) Steps:
//   For each system service (fs, net, etc):
//     1. Register a port for each process (init is the only process with
//     permissions to assign SYSTEM ports. Other processes can assign ports, but
//     none that are reserved for system use (example: net, fs))
//     2. Launch the service

// TODO: Dynamically allocate this in the future
struct TarHeader *headers[32];

void main(int argc, char **argv)
{
    if (argc != 2)
    {
        // TODO: abort()
    }

    int entry_count = tar_parse_headers(strtoul(argv[1]), &headers);

    for (int i = 0; i < entry_count; i++)
    {
        auto tar_content = (uint64_t)headers[i] + TAR_BLOCK_SIZE;
        syscall_create_process(
            tar_content,
            5);  // TODO: dynamically assign port based on application
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
