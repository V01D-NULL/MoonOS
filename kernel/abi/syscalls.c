#define PR_MODULE "syscall"
#include "syscalls.h"
#include <base/base-types.h>
#include <base/string.h>
#include <cpu.h>
#include <ipc/ipc.h>
#include <mm/addr.h>
#include <mm/virt.h>
#include <moon-extra/compiler.h>
#include <moon-io/io.h>
#include <panic.h>
#include <printk.h>
#include <sched/scheduler.h>

#define Type uint64_t
#define ignore Type UNIQUE_NAME(ignore)

// Syscall stub
int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore)
{
    return -1;
}

int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
{
    (void)fd;

    // Note: is_user_address takes buff = NULL into account
    if (unlikely(is_user_address(buff) == false))
        return 1;

    string_view ptr = (string_view)buff;

    if (strlen(ptr) != len)
        return 2;

    trace(TRACE_SYSCALL, "%s", ptr);

    return 0;
}

int sys_ipc_send(Type to, Type buff, ignore, ignore, ignore, ignore)
{
    if (unlikely(is_user_address(buff) == false))
        return 1;

    auto task    = sched_get(to);
    auto message = (struct UserIpcMessage *)buff;

    auto payload = alloc(message->message.payload_size);
    memcpy(payload, message->message.payload, message->message.payload_size);

    ipc_send(to, payload, message->message.payload_size);

    return 0;
}

int sys_ipc_receive(Type buff, ignore, ignore, ignore, ignore, ignore)
{
    auto response = (struct UserIpcMessage *)buff;

    auto task = sched_current();
    if (!task)
        return 1;

    auto message = ipc_receive(task->message_queue);
    if (message == NULL)
        return 2;

    response->sender               = message->sender;
    response->message.type         = IPC_MESSAGE_TYPE_RESPONSE;
    response->message.payload_size = message->payload_size;

    void *payload_dest =
        (void *)response + offsetof(struct UserIpcMessage, message.payload);
    memcpy(payload_dest, message->payload, message->payload_size);

    trace(TRACE_SYSCALL, "%#lx\n", payload_dest);

    free(message->payload, message->payload_size);

    return 0;
}
