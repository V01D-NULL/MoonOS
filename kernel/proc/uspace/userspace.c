#include "userspace.h"
#include "syscalls.h"
#include <amd64/msr.h>
#include <ktypes.h>

extern void enter_ring3_sysret(void *addr, size_t ustack);

NORETURN void enter_ring3(void *addr, size_t stack)
{
    enter_ring3_sysret(addr, stack);
    __builtin_unreachable();
}