#define PR_MODULE "ipc"
#include "ipc.h"
#include <printk.h>
#include <sched/scheduler.h>

void ipc_send(uint64_t to, uint64_t payload, uint64_t payload_size)
{
    ExecutionSpace *target_space  = sched_get(to);
    ExecutionSpace *current_space = sched_current();

    if (!target_space || !current_space)
    {
        trace(TRACE_IPC,
              "IPC send failed: unable to get current or target execution space"
              "(to: %d, target: %p, current: %p)\n",
              to,
              target_space,
              current_space);
        return;
    }

    struct IpcMessage message = {
        .sender       = current_space->pid,
        .receiver     = to,
        .payload      = payload,
        .payload_size = payload_size,
    };

    trace(TRACE_IPC,
          "Enqueuing IPC send: %#lx (to: %d, from: %d)\n",
          payload,
          to,
          message.sender);

    push(&target_space->message_queue, message);
}

struct IpcMessage *ipc_receive(IpcMessageQueue message_queue)
{
    if (size(&message_queue) == 0)
        return NULL;

    struct IpcMessage *message = erase(&message_queue, 0);

    trace(TRACE_IPC,
          "IPC receive: %#lx (to: %d, from: %d)\n",
          message->payload,
          message->receiver,
          message->sender);

    return message;
}