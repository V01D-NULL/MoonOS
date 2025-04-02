#ifndef ES_H
#define ES_H

#include <ipc/message-queue.h>
#include <moon-ds/containers.h>
#include <paging/paging.h>
#include <service/execution-context/ec.h>

typedef struct
{
    int              pid;
    struct Pml      *vm_space;
    uint64_t         stack_pointer;
    ExecutionContext ec;
    IpcMessageQueue  message_queue;
} ExecutionSpace;

#endif  // ES_H