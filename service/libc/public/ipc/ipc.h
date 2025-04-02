#ifndef PUBLIC_IPC_H
#define PUBLIC_IPC_H

#include <stdint.h>

typedef enum
{
    IPC_MESSAGE_TYPE_NONE = 0,
    IPC_MESSAGE_TYPE_REQUEST,
    IPC_MESSAGE_TYPE_RESPONSE,
} IpcMessageType;

struct IpcMessage
{
    uint64_t sender;
    struct
    {
        IpcMessageType type;
        uint64_t       payload;
        uint64_t       payload_size;
    } message;
};

void ipc_send(uint64_t to, uint64_t payload, uint64_t payload_size);
struct IpcMessage *ipc_receive(void);
void               ipc_ack(void);

#endif  // PUBLIC_IPC_H