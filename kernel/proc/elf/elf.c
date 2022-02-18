#include "elf.h"
#include <panic.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mm.h>

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf);
static task_t elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, bool do_panic);

task_t load_elf(const uint8_t *elf, bool do_panic)
{
    // elf = (const uint8_t *)from_higher_half((uintptr_t)elf, DATA);
    Elf64_Ehdr ehdr = elf_verify_ehdr(&elf);

    printk("elf", "Elf version: %d\n", ehdr.e_version);
    printk("elf", "Entry point: %X\n", ehdr.e_entry);

    return elf_parse_phdr(&elf, &ehdr, do_panic);
}

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf)
{
    Elf64_Ehdr ehdr = *(Elf64_Ehdr *)(*elf);
    if (!IS_ELF(ehdr))
    {
        panic("Invalid ELF magic");
    }
    printk("elf", "Valid ELF magic\n");

    panic_if(ehdr.e_ident[EI_CLASS] != ELFCLASS64, "Elf must be 64 bit");
    panic_if(ehdr.e_ident[EI_OSABI] != ELFOSABI_SYSV, "Expected a sysv abi conform elf (ELFOSABI_SYSV (0)), got %d", ehdr.e_ident[EI_OSABI]);
    panic_if(ehdr.e_type != ET_EXEC, "Expected elf type ET_EXEC(2), got %d", ehdr.e_type);

    return ehdr;
}

static task_t elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, bool do_panic)
{
    Elf64_Half phdr_entries = ehdr->e_phnum;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(*elf + ehdr->e_phoff);
    task_t task = new_task("init daemon", ehdr->e_entry);
    printk("elf", "task pagemap: %p\n", task.pagemap);

    bool found_ptload = false;
    vmm_copy_kernel_mappings(task);
    for (Elf64_Half i = 0; i < phdr_entries; i++)
    {
        if (phdr->p_type == PT_LOAD)
        {
            found_ptload = true;
            size_t num_pages = ALIGN_UP(phdr->p_memsz) / 4096;

            printk("elf", "Found loadable segment\n");
            printk("elf", "Virtual mapping: 0x%lX (%d bytes | %d pages)\n", phdr->p_vaddr, phdr->p_memsz, num_pages);

            uintptr_t elf_base = (uintptr_t)pmm_alloc_range(num_pages).base;
            for (uint64_t i = 0; i < num_pages; i++)
            {
                vmm_map(task.pagemap, phdr->p_vaddr + (i * PAGE_SIZE), elf_base + (i * PAGE_SIZE), MAP_USER);
            }

            memcpy((uint8_t *)phdr->p_vaddr, (const uint8_t *)*elf + phdr->p_offset, phdr->p_filesz);
            memset((void *)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_filesz - phdr->p_memsz);
        }

        phdr += ehdr->e_phentsize;
    }

    if (!found_ptload && do_panic)
        panic("Could not find a PT_LOAD segment!");

    vmm_map_range(vmm_as_range(task.ustack, task.ustack + 8192, VMEM_DIRECT_MAPPING), MAP_USER, task.pagemap);
    add_task(task);
    return task;
}