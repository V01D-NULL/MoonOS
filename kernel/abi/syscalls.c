#include "syscalls.h"
#include <base/base-types.h>
#include <base/string.h>
#include <cpu.h>
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

    if (buff == 0)
        return -1;

    string_view ptr = (string_view)buff;
    debug(true, "len %d | strlen: %d\n", len, strlen(ptr));

    asm volatile("int3");
    trace("%s", ptr);
    // for (auto i = 0; i < len; i++)
    //     trace("%c", ptr[i]);

    return 0;
}
