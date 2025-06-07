#define PR_MODULE "ipc"
#include "ipc.h"
#include <mm/virt.h>
#include <moon-extra/range.h>
#include <printk.h>
#include <sched/scheduler.h>

omap(int, int) port_to_pid;

void ipc_init(void)
{
    init(&port_to_pid);
}

bool ipc_assign_port(ExecutionSpace *space, int port)
{
    if (space->port != -1 || get(&port_to_pid, port) != NULL)
    {
        return false;
    }

    space->port = port;
    init(&space->message_queue);
    insert(&port_to_pid, space->port, space->pid);
    Range ipc_region = {
        .base  = IPC_MESSAGE_REGION_BASE,
        .limit = IPC_MESSAGE_REGION_BASE + IPC_MESSAGE_REGION_SZ,
    };
    arch_map_range(space->vm_space, ipc_region, MAP_USER_RW, 0);
    return true;
}

void ipc_send(uint64_t to, uint64_t payload, uint64_t payload_size)
{
    int *target_pid = get(&port_to_pid, to);
    if (target_pid == NULL)
    {
        trace(TRACE_IPC,
              "IPC send failed: requested port '%d' does not exist\n",
              to);
        return;
    }

    ExecutionSpace *target_space  = sched_get(*target_pid);
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