#ifndef ES_H
#define ES_H

#include <paging/paging.h>
#include <service/execution-context/ec.h>

typedef struct
{
    struct Pml      *vm_space;
    ExecutionContext ec;
} ExecutionSpace;

#endif  // ES_H