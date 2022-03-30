#include "syscalls.h"
#include <libk/kprintf.h>
#include <stdint.h>

// Note: The string should already be formatted when calling syscall_log()
// meaning it does NOT have printf() functionality.
void syscall_log(const char *msg)
{
    asm volatile(
        "syscall\n"
    );
}

void foo(void)
{
    asm volatile(
        "syscall\n"
    );
    for(;;);
}