#include <ipc/ipc.h>
#include <memory.h>
#include <stddef.h>

#define IPC_MESSAGE_REGION_BASE \
    0x800000  // 8MiB - Hardcoded for now. In the future this will be
              // dynamically allocated

static struct IpcMessage *ipc_message_receive_region = NULL;
static struct IpcMessage *ipc_message_send_region    = NULL;

static struct IpcMessage *ipc_init_message_region(void);

void ipc_init(void)
{
    struct IpcMessage *base    = (struct IpcMessage *)IPC_MESSAGE_REGION_BASE;
    ipc_message_receive_region = base;
    ipc_message_send_region    = (struct IpcMessage *)(((uint64_t)base) + 4096);
}

void ipc_send(uint64_t to, uint64_t payload, uint64_t payload_size)
{
    ipc_message_send_region->message.type         = IPC_MESSAGE_TYPE_REQUEST;
    ipc_message_send_region->message.payload      = payload;
    ipc_message_send_region->message.payload_size = payload_size;

    syscall_ipc_send(to, ipc_message_send_region);
}

struct IpcMessage *ipc_receive(void)
{
    syscall_ipc_receive(
        ipc_message_receive_region, sizeof(ipc_message_receive_region));

    return ipc_message_receive_region;
}

void ipc_ack(void)
{
    memset(ipc_message_receive_region, 0, 4096);
}
