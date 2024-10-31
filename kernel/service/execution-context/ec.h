#ifndef EC_H
#define EC_H

#include <sys/context_switch.h>

typedef struct
{
    uint64_t                   entry;
    struct arch_task_registers registers;
} ExecutionContext;

#endif  // EC_H