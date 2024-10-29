#define PR_MODULE "execution-space"
#include "create.h"
#include <mm/virt.h>
#include <printk.h>
#include "loader/elf.h"

EsCreateResult create_execution_space(const uint8_t *elf_pointer)
{
    ExecutionSpace execution_space = {
        .vm_space = arch_create_new_pagemap(),
    };
    arch_copy_kernel_mappings(execution_space.vm_space);

    auto entry_point = TRY_UNWRAP(
        _load_elf(elf_pointer, execution_space.vm_space), EsCreateResult);

    trace(TRACE_ELF, "Entry point: %#lx\n", entry_point);

    return Okay(EsCreateResult, execution_space);
}