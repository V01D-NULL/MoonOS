#include "syscalls.h"
#include <libk/kprintf.h>

// Note: The string should already be formatted when calling syscall_log()
// meaning it does NOT have printf() functionality.
void syscall_log(const char *msg)
{
    // __asm__ volatile("syscall") // ....
}