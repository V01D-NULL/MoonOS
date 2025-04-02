#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <moon-ds/containers.h>

struct IpcMessage
{
    uint64_t sender;
    uint64_t receiver;
    uint64_t payload;
    uint64_t payload_size;
};

typedef cc_vec(struct IpcMessage) IpcMessageQueue;

#endif  // MESSAGE_QUEUE_H