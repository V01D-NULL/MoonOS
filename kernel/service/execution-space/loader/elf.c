#include "elf.h"
#include <printk.h>

static bool has_valid_attributes(const Elf64_Ehdr ehdr)
{
    return ehdr.e_ident[EI_CLASS] == ELFCLASS64 &&
           ehdr.e_ident[EI_OSABI] == ELFOSABI_SYSV && ehdr.e_type == ET_EXEC;
}

static ElfFile parse_elf_file(const uint8_t *elf)
{
    ElfFile elf_file;
    elf_file.ehdr  = *(Elf64_Ehdr *)elf;
    elf_file.phdrs = (Elf64_Phdr *)(elf + elf_file.ehdr.e_phoff);
    elf_file.shdrs = (Elf64_Shdr *)(elf + elf_file.ehdr.e_shoff);
    elf_file.shstrtab =
        (char *)(elf + elf_file.shdrs[elf_file.ehdr.e_shstrndx].sh_offset);
    return elf_file;
}

ElfLoaderResult _load_elf(const uint8_t *elf_pointer)
{
    ElfFile elf = parse_elf_file(elf_pointer);

    if (!IS_ELF(elf.ehdr) || !has_valid_attributes(elf.ehdr))
    {
        return Error(
            ElfLoaderResult, "ELF file is not valid or has invalid attributes");
    }

    return Okay(ElfLoaderResult, (ElfLoaderResult){});
}