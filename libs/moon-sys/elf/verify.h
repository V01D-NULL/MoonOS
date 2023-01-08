#ifndef ELF_VERIFY_H
#define ELF_VERIFY_H

#include "moon-sys/elf/elf64.h"
#include "moon-sys/elf/elf_common.h"

Elf64_Ehdr elf_verify_ehdr(const uint8_t *elf);

#endif // ELF_VERIFY_H