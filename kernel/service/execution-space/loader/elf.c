#define PR_MODULE "elf"
#include "elf.h"
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
    elf_file.ehdr  = *(Elf64_Ehdr *)elf;
    elf_file.phdrs = (Elf64_Phdr *)(elf + elf_file.ehdr.e_phoff);
    elf_file.shdrs = (Elf64_Shdr *)(elf + elf_file.ehdr.e_shoff);
    elf_file.shstrtab =
        (char *)(elf + elf_file.shdrs[elf_file.ehdr.e_shstrndx].sh_offset);
    return elf_file;
}

ElfLoaderResult _load_elf(const uint8_t *elf_pointer, struct Pml *space)
{
    ElfFile elf = parse_elf_file(elf_pointer);

    if (!IS_ELF(elf.ehdr) || !has_valid_attributes(elf.ehdr))
    {
        return Error(ElfLoaderResult,
                     "ELF file is not valid or is configured incorrectly");
    }

    // Keep track of virtual addresses that have been mapped already.
    // This is useful for handling overlapping segments.
    cc_omap(virt_t, phys_t) page_map;
    init(&page_map);

    for (int i = 0; i < elf.ehdr.e_phnum; i++)
    {
        Elf64_Phdr phdr = elf.phdrs[i];

        if (phdr.p_type != PT_LOAD)
            continue;

        auto previous_mapping_phys_address =
            get(&page_map, phdr.p_vaddr & ~(PAGE_SIZE - 1));

        if (previous_mapping_phys_address)
        {
            trace(TRACE_ELF,
                  "Already mapped %#lx to physical address %#lx...copying data "
                  "instead\n",
                  phdr.p_vaddr,
                  *previous_mapping_phys_address);

            // Calculate the offset within the existing page
            size_t offset = phdr.p_vaddr % PAGE_SIZE;

            // Copy the new data to the correct offset
            memcpy((char *)*previous_mapping_phys_address + offset,
                   elf_pointer + phdr.p_offset,
                   phdr.p_filesz);

            if (phdr.p_memsz > phdr.p_filesz)
            {
                memset((char *)(*previous_mapping_phys_address + offset +
                                phdr.p_filesz),
                       0,
                       phdr.p_memsz - phdr.p_filesz);
            }
        }
        else
        {
            trace(TRACE_ELF, "Loading segment at %#lx\n", phdr.p_vaddr);

            auto page = arch_alloc_page();

            int flags = 0;
            if (phdr.p_flags & PF_R)
                flags |= MAP_USER_RO;

            if (phdr.p_flags & PF_W)
                flags |= MAP_USER_RW;

            arch_map_page(space, phdr.p_vaddr, page, flags);

            memcpy((void *)page, elf_pointer + phdr.p_offset, phdr.p_filesz);

            if (phdr.p_memsz > phdr.p_filesz)
            {
                memset((void *)(page + phdr.p_filesz),
                       0,
                       phdr.p_memsz - phdr.p_filesz);
            }

            insert(&page_map, phdr.p_vaddr, page);
        }
    }

    cleanup(&page_map);
    return Okay(ElfLoaderResult, elf.ehdr.e_entry);
}
