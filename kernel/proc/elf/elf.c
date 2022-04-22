#include "elf.h"
#include <panic.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mm.h>
#include <ktypes.h>

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf);
static task_t elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, struct elf_loader_args args);

task_t load_elf(const uint8_t *elf, struct elf_loader_args args)
{
    Elf64_Ehdr ehdr = elf_verify_ehdr(&elf);
    return elf_parse_phdr(&elf, &ehdr, args);
}

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf)
{
    Elf64_Ehdr ehdr = *(Elf64_Ehdr *)(*elf);

    if (!IS_ELF(ehdr))
    {
        return (Elf64_Ehdr){};
    }

    bool valid_status;
    valid_status = ehdr.e_ident[EI_CLASS] == ELFCLASS64 &&
                   ehdr.e_ident[EI_OSABI] == ELFOSABI_SYSV &&
                   ehdr.e_type == ET_EXEC;

    return valid_status ? ehdr : (Elf64_Ehdr){};
}

static task_t elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, struct elf_loader_args args)
{
    Elf64_Half phdr_entries = ehdr->e_phnum;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(*elf + ehdr->e_phoff);
    task_t task = new_task(args.descriptor, ehdr->e_entry);

    size_t elf_mapping_offset = phdr->p_vaddr;
    bool found_ptload = false;
    copy_kernel_mappings(task);

    for (Elf64_Half i = 0; i < phdr_entries; i++)
    {
        if (phdr->p_type == PT_LOAD)
        {
            found_ptload = true;
            size_t num_pages = ALIGN_UP(phdr->p_memsz) / 4096;

            // printk("elf", "Virtual mapping: 0x%lX (%d bytes | %d pages)\n", phdr->p_vaddr, phdr->p_memsz, num_pages);

            for (uint64_t i = 0; i < num_pages; i++)
            {
                elf_mapping_offset += (i * PAGE_SIZE);
                v_map(task.pagemap, elf_mapping_offset, elf_mapping_offset, MAP_USER_RW);
            }

            memcpy((uint8_t *)phdr->p_vaddr, (const uint8_t *)*elf + phdr->p_offset, phdr->p_filesz);
            memset((void *)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_filesz - phdr->p_memsz);
        }

        phdr += ehdr->e_phentsize;
    }

    if (!found_ptload)
    {
        panic_if(args.do_panic, "Could not find a PT_LOAD segment!");
        return (task_t){};
    }

    // Stack memory (8KiB)
    task.ustack = elf_mapping_offset;
    for (size_t i = 0; i <= 2; i++)
    {
        auto addr = elf_mapping_offset = i * PAGE_SIZE + task.ustack;
        v_map_fast(task.pagemap, addr, addr, MAP_USER_RW);
    }
    // Todo: Shadow stack page

    // v_map_range_fast(as_vmm_range(0x0, 0x400000, $identity_vma), MAP_KERN, task.pagemap);
    // v_map_range_fast(as_vmm_range(elf_mapping_offset, (4 * GB) - elf_mapping_offset, $identity_vma), MAP_KERN, task.pagemap);

    return task;
}