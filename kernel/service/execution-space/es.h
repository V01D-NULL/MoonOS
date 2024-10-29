#ifndef ES_H
#define ES_H

#include <paging/paging.h>
#include <service/execution-context/ec.h>

typedef struct
{
    struct Pml      *vm_space;
    uint64_t         stack_pointer;
    ExecutionContext ec;
} ExecutionSpace;

#endif  // ES_H