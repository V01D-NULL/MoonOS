#include "elf.h"
#include <panic.h>
#include <printk.h>
#include <devices/serial/serial.h>
#include <mm/vmm.h>

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf);
static void elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, bool do_panic);

void load_elf(const uint8_t *elf, bool do_panic)
{
    Elf64_Ehdr ehdr = elf_verify_ehdr(&elf);

    printk("elf", "Elf version: %d\n", ehdr.e_version);
    printk("elf", "Entry point: %X\n", ehdr.e_entry);

    elf_parse_phdr(&elf, &ehdr, do_panic);
}

static Elf64_Ehdr elf_verify_ehdr(const uint8_t **elf)
{
    Elf64_Ehdr ehdr = *(Elf64_Ehdr*)(*elf);
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

static void elf_parse_phdr(const uint8_t **elf, Elf64_Ehdr *ehdr, bool do_panic)
{
    Elf64_Half phdr_entries = ehdr->e_phnum;
    Elf64_Phdr *phdr = (Elf64_Phdr*)(*elf + ehdr->e_phoff);
    
    bool found_ptload = false;
    for (Elf64_Half i = 0; i < phdr_entries; i++)
    {
        if (phdr->p_type == PT_LOAD)
        {
            found_ptload = true;
            printk("elf", "Found loadable segment\n");
            printk("elf", "Virtual mapping: 0x%lX (%d bytes | %d pages)\n", phdr->p_vaddr, phdr->p_memsz, phdr->p_memsz / 4096);
            
            
        }
        
        phdr += ehdr->e_phentsize;
    }

    if (!found_ptload && do_panic)
        panic("Could not find a PT_LOAD segment!");
}