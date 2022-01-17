#ifndef ELF_H
#define ELF_H

#include "elf64.h"
#include "elf_common.h"
#include <ktypes.h>
#include <proc/tasking/task.h>

task_t load_elf(const uint8_t *elf, bool do_panic);

#endif // ELF_H