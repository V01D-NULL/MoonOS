#include "strace.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <drivers/io/serial.h>
#include <ds/linked_list.h>

void backtrace_stack(int frames)
{
    struct ds_default_list *stackframe;
    __asm__ ("mov %%rbp, %0" : "=r"(stackframe));
    
    size_t i = 0;
    while (i < frames && stackframe)
    {
        debug(false, "%llx\n", stackframe->value);       
        ds_default_next(&stackframe);
        i++;
    }
}