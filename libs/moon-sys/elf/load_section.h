#ifndef ELF_LOAD_SECTION_H
#define ELF_LOAD_SECTION_H

#include "moon-sys/elf/elf64.h"
#include "moon-sys/elf/elf_common.h"

bool elf_load_section(void *elf, string_view target, Elf64_Ehdr ehdr);

#endif // ELF_LOAD_SECTION_H