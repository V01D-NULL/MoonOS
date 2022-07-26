#ifndef ELF_H
#define ELF_H

#include "elf64.h"
#include "elf_common.h"
#include <ktypes.h>
#include <proc/sched/task.h>

struct elf_loader_args
{
    bool do_panic;          // Panic if something goes wrong? (Usually used for critical services such as kernel daemons)
    bool identity_map;      // Should the ELF's entry point be identity mapped or not?
    string_view descriptor; // What should the task be known as?
};

task_t load_elf(const uint8_t *elf, struct elf_loader_args args);

#endif // ELF_H