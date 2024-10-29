#include "create.h"
#include "loader/elf.h"

EsCreateResult create_execution_space(const uint8_t *elf_pointer)
{
    TRY_UNWRAP(_load_elf(elf_pointer), EsCreateResult);

    return Okay(EsCreateResult, (ExecutionSpace){});
}