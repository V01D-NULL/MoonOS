#include "create.h"

ExecutionContext create_ec(uint64_t entry, uint64_t sp)
{
    return (ExecutionContext){
        .entry = entry,
        .registers =
            {
                .ip        = entry,
                .registers = {.rbp = sp, 0},
                .rsp       = sp - 2048,
            },
    };
}