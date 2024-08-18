#define PR_MODULE "syscall"
#include "syscalls.h"
#include <base/base-types.h>
#include <base/string.h>
#include <cpu.h>
#include <mm/addr.h>
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
        return -1;

    string_view ptr = (string_view)buff;

    if (strlen(ptr) != len)
        return -2;

    trace(TRACE_SYSCALL, "%s", ptr);

    return 0;
}
