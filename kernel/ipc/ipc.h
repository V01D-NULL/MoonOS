#ifndef IPC_H
#define IPC_H

#include <base/base-types.h>
#include <moon-ds/containers.h>
#include <service/execution-space/es.h>
// #include "message-queue.h"

#include <mm/addr.h>

#define IPC_MESSAGE_REGION_BASE MiB(8)
#define IPC_MESSAGE_REGION_SZ KiB(8)

typedef enum
{
    IPC_MESSAGE_TYPE_NONE = 0,
    IPC_MESSAGE_TYPE_REQUEST,
    IPC_MESSAGE_TYPE_RESPONSE,
} IpcMessageType;

struct UserIpcMessage
{
    uint64_t sender;
    struct
    {
        IpcMessageType type;
        uint64_t       payload;
        uint64_t       payload_size;
    } message;
};

void               ipc_send(uint64_t to, uint64_t buff, uint64_t buff_len);
struct IpcMessage *ipc_receive(IpcMessageQueue message_queue);

#endif  // IPC_H