#ifndef ELF_H
#define ELF_H

#include <moon-sys/elf/elf64.h>
#include <moon-sys/elf/elf_common.h>
#include <base/base-types.h>
#include <sched/task.h>

Task load_elf(const uint8_t *elf, string_view descriptor, bool panic_on_fail);

#endif // ELF_H