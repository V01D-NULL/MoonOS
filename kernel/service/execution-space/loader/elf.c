#define PR_MODULE "elf"
#include "elf.h"
#include <base/align.h>
#include <base/mem.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <moon-ds/containers.h>
#include <moon-extra/iterator.h>
#include <printk.h>

static bool has_valid_attributes(const Elf64_Ehdr ehdr)
{
    return ehdr.e_ident[EI_CLASS] == ELFCLASS64 &&
           ehdr.e_ident[EI_OSABI] == ELFOSABI_SYSV && ehdr.e_type == ET_EXEC;
}

static ElfFile parse_elf_file(const uint8_t *elf)
{
    ElfFile elf_file;
    elf_file.base  = (uint8_t *)elf;
    elf_file.ehdr  = *(Elf64_Ehdr *)elf;
    elf_file.phdrs = (Elf64_Phdr *)(elf + elf_file.ehdr.e_phoff);
    elf_file.shdrs = (Elf64_Shdr *)(elf + elf_file.ehdr.e_shoff);
    elf_file.shstrtab =
        (Elf64_Shdr *)(elf_file.ehdr.e_shoff +
                       elf_file.ehdr.e_shstrndx * elf_file.ehdr.e_shentsize);

    return elf_file;
}

void load_segment(uint8_t *elf_base, struct Pml *space, Elf64_Phdr *phdr,
                  uint64_t page_number);

void load_section_headers(ElfFile elf, struct Pml *space);

ElfLoaderResult load_elf(const uint8_t *elf_pointer, struct Pml *space)
{
    ElfFile elf = parse_elf_file(elf_pointer);

    if (!IS_ELF(elf.ehdr) || !has_valid_attributes(elf.ehdr))
    {
        return Error(ElfLoaderResult,
                     "ELF file is not valid or is configured incorrectly");
    }

    for (Elf64_Half i = 0; i < elf.ehdr.e_phnum; i++)
    {
        if (elf.phdrs->p_type != PT_LOAD)
            continue;

        size_t num_pages = ALIGN_UP(elf.phdrs->p_memsz) / PAGE_SIZE;

        for (uint64_t j = 0; j < num_pages; j++)
            load_segment(elf.base, space, elf.phdrs, j);

        elf.phdrs = (Elf64_Phdr *)elf.phdrs + elf.ehdr.e_phentsize;
    }

    load_section_headers(elf, space);

    return Okay(ElfLoaderResult, elf.ehdr.e_entry);
}

void load_segment(uint8_t *elf_base, struct Pml *space, Elf64_Phdr *phdr,
                  uint64_t page_number)
{
    uintptr_t phys = arch_alloc_page();
    if (!phys)
        return Error(
            ElfLoaderResult, "Failed to allocate page for PT_LOAD segment");

    uintptr_t vaddr = phdr->p_vaddr + (page_number * PAGE_SIZE);
    arch_map_page(space, vaddr, pa(phys), MAP_USER_RW);

    // Calculate the offset and size to copy
    size_t page_offset = page_number * PAGE_SIZE;
    size_t copy_size   = PAGE_SIZE;

    if (page_offset >= phdr->p_filesz)
    {
        // All remaining pages should be zeroed
        memset(phys, 0, PAGE_SIZE);
        return;
    }

    if (page_offset + PAGE_SIZE > phdr->p_filesz)
        copy_size = phdr->p_filesz - page_offset;

    memcpy(phys, elf_base + phdr->p_offset + page_offset, copy_size);

    if (copy_size < PAGE_SIZE)
        memset(phys + copy_size, 0, PAGE_SIZE - copy_size);
}

void load_section_headers(ElfFile elf, struct Pml *space)
{
    Elf64_Shdr *shstrtab =
        (Elf64_Shdr *)(elf.base + elf.ehdr.e_shoff +
                       elf.ehdr.e_shstrndx * elf.ehdr.e_shentsize);

    const char *section_names = (const char *)(elf.base + shstrtab->sh_offset);

    for (int section_idx = 0; section_idx < elf.ehdr.e_shnum; section_idx++)
    {
        Elf64_Shdr *shdr = (Elf64_Shdr *)(elf.base + elf.ehdr.e_shoff +
                                          section_idx * elf.ehdr.e_shentsize);
        debug(true,
              "Section %d: %s\n",
              section_idx,
              &section_names[shdr->sh_name]);

        if (!strncmp(&section_names[shdr->sh_name], ".rodata", 7))
        {
            debug(true, "Found .rodata section\n");
            uintptr_t rodata_vaddr = shdr->sh_addr;
            uintptr_t rodata_phys  = arch_translate(space, rodata_vaddr);
            debug(true, "Copying .rodata to 0x%lx\n", rodata_phys);
            memcpy((void *)rodata_phys,
                   (const void *)(elf.base + shdr->sh_offset),
                   shdr->sh_size);
        }
    }
}
