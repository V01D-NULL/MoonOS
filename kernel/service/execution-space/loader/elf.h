#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <base/base-types.h>
#include <moon-extra/result.h>
#include <moon-sys/elf/elf64.h>
#include <moon-sys/elf/elf_common.h>

typedef struct
{
} LoadedElf;

typedef struct
{
    Elf64_Ehdr  ehdr;
    Elf64_Phdr *phdrs;
    Elf64_Shdr *shdrs;
    char       *shstrtab;
} ElfFile;

typedef Result(LoadedElf, Nullish) ElfLoaderResult;

ElfLoaderResult _load_elf(const uint8_t *elf_pointer);

#endif  // ELF_LOADER_H