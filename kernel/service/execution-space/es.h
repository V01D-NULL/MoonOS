#ifndef ES_H
#define ES_H

#include <paging/paging.h>

typedef struct
{
    struct Pml *vm_space;
} ExecutionSpace;

#endif  // ES_H