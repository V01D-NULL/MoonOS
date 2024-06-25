// #define PR_MODULE "elf"

// #include "elf.h"
// #include <panic.h>
// #include <printk.h>
// #include <moon-io/serial.h>
// #include <mm/virt.h>
// #include <mm/phys.h>
// #include <base/base-types.h>
// #include <base/string.h>
// #include <base/align.h>
// #include <moon-sys/elf/load_section.h>
// #include <moon-sys/elf/verify.h>

// static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, string_view descriptor, bool panic_on_fail);

// Task load_elf(const uint8_t *elf, string_view descriptor, bool panic_on_fail)
// {
//     Elf64_Ehdr ehdr = elf_verify_ehdr(elf);
//     return elf_parse_phdr(elf, ehdr, descriptor, panic_on_fail);
// }

// static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, string_view descriptor, bool panic_on_fail)
// {
//     Elf64_Half phdr_entries = ehdr.e_phnum;
//     Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr.e_phoff);

//     Task task = create_task_struct(descriptor, ehdr.e_entry);

//     size_t elf_mapping_offset = 0;
//     bool found_ptload = false;
//     arch_copy_kernel_mappings(task);
//     auto phys = arch_alloc_page();
//     if (!phys) {
//         panic_if(panic_on_fail, "Failed to allocate initial page for ELF loading!");
//         return (Task){};
//     }

// for (Elf64_Half i = 0; i < phdr_entries; i++)
// {
//     if (phdr->p_type == PT_LOAD)
//     {
//         found_ptload = true;
//         size_t num_pages = ALIGN_UP(phdr->p_memsz) / PAGE_SIZE;

//         for (uint64_t j = 0; j < num_pages; j++)
//         {
//             if (j > 0) {
//                 phys = arch_alloc_page();
//                 if (!phys) {
//                     panic_if(panic_on_fail, "Failed to allocate page for PT_LOAD segment!");
//                     return (Task){};
//                 }
//             }
//             elf_mapping_offset = phdr->p_vaddr + (j * PAGE_SIZE);
//             arch_quickmap_page(task.pagemap, elf_mapping_offset, (size_t)phys, MAP_USER_RW);
//         }

//         memcpy((uint8_t *)phys, (const uint8_t *)elf + phdr->p_offset, phdr->p_filesz);
//         memset((void *)phys + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);

//         Elf64_Shdr *shstrtab = (Elf64_Shdr *)(elf + ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize);
//         const char *section_names = (const char *)(elf + shstrtab->sh_offset);

//         for (int k = 0; k < ehdr.e_shnum; k++)
//         {
//             Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + ehdr.e_shoff + k * ehdr.e_shentsize);
//             debug(true, "Section %d: %s\n", k, &section_names[shdr->sh_name]);

//             // // Check for .text section
//             // if (!strncmp(&section_names[shdr->sh_name], ".text", 5))
//             // {
//             //     debug(true, "Found .text section\n");
//             //     memcpy((uint8_t *)(phys + shdr->sh_addr - phdr->p_vaddr), (const uint8_t *)(elf + shdr->sh_offset), shdr->sh_size);
//             // }

//             // Check for .rodata section
//             if (!strncmp(&section_names[shdr->sh_name], ".rodata", 7))
//             {
//                 debug(true, "Found .rodata section\n");
//                 memcpy((uint8_t *)(phys + shdr->sh_addr - phdr->p_vaddr), (const uint8_t *)(elf + shdr->sh_offset), shdr->sh_size);
//             }
//         }
//     }
//     phdr = (Elf64_Phdr *)((uint8_t *)phdr + ehdr.e_phentsize);
// }

//     if (!found_ptload)
//     {
//         panic_if(panic_on_fail, "Could not find a PT_LOAD segment!");
//         return (Task){};
//     }

//     // Stack memory (8KiB)
//     task.ustack = elf_mapping_offset + PAGE_SIZE;
//     for (size_t i = 1; i <= 2; i++)
//     {
//         auto addr = elf_mapping_offset + i * PAGE_SIZE;
// 		debug(true, "addr: %p\n", addr);
//         arch_map_page(task.pagemap, addr, addr, MAP_USER_RW);
//     }
//     // Todo: Shadow stack page
// 	debug(true, "task.ustack: %lx\n", task.ustack);

//     return task;
// }
#define PR_MODULE "elf"

#include "elf.h"
#include <panic.h>
#include <printk.h>
#include <moon-io/serial.h>
#include <mm/virt.h>
#include <mm/phys.h>
#include <base/base-types.h>
#include <base/string.h>
#include <base/align.h>
#include <moon-sys/elf/load_section.h>
#include <moon-sys/elf/verify.h>

static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, string_view descriptor, bool panic_on_fail);

Task load_elf(const uint8_t *elf, string_view descriptor, bool panic_on_fail)
{
    Elf64_Ehdr ehdr = elf_verify_ehdr(elf);
    return elf_parse_phdr(elf, ehdr, descriptor, panic_on_fail);
}

static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, string_view descriptor, bool panic_on_fail)
{
    debug(true, "Loading %s\n", descriptor);
    Elf64_Half phdr_entries = ehdr.e_phnum;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr.e_phoff);

    Task task = create_task_struct(descriptor, ehdr.e_entry);

    bool found_ptload = false;
    arch_copy_kernel_mappings(task);

    for (Elf64_Half i = 0; i < phdr_entries; i++)
    {
        if (phdr->p_type == PT_LOAD)
        {
            found_ptload = true;
            size_t num_pages = ALIGN_UP(phdr->p_memsz) / PAGE_SIZE;
            uintptr_t segment_vaddr = phdr->p_vaddr;

            for (uint64_t j = 0; j < num_pages; j++)
            {
                uintptr_t phys = arch_alloc_page();
                if (!phys) {
                    panic_if(panic_on_fail, "Failed to allocate page for PT_LOAD segment!");
                    return (Task){};
                }
                
                uintptr_t vaddr = segment_vaddr + (j * PAGE_SIZE);
                arch_quickmap_page(task.pagemap, vaddr, phys, MAP_USER_RW);

                // Calculate the offset and size to copy
                size_t page_offset = j * PAGE_SIZE;
                size_t copy_size = PAGE_SIZE;

                if (page_offset >= phdr->p_filesz) {
                    // All remaining pages should be zeroed
                    memset((void *)phys, 0, PAGE_SIZE);
                } else {
                    if (page_offset + PAGE_SIZE > phdr->p_filesz) {
                        copy_size = phdr->p_filesz - page_offset;
                    }
                    memcpy((void *)phys, (const void *)(elf + phdr->p_offset + page_offset), copy_size);
                    if (copy_size < PAGE_SIZE) {
                        memset((void *)(phys + copy_size), 0, PAGE_SIZE - copy_size);
                    }
                }
            }

            debug(true, "Mapped PT_LOAD segment at vaddr 0x%lx, size 0x%lx\n", phdr->p_vaddr, phdr->p_memsz);

            Elf64_Shdr *shstrtab = (Elf64_Shdr *)(elf + ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize);
            const char *section_names = (const char *)(elf + shstrtab->sh_offset);

            for (int k = 0; k < ehdr.e_shnum; k++)
            {
                Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + ehdr.e_shoff + k * ehdr.e_shentsize);
                debug(true, "Section %d: %s\n", k, &section_names[shdr->sh_name]);

                if (!strncmp(&section_names[shdr->sh_name], ".text", 5))
                {
                    debug(true, "Found .text section\n");
                    uintptr_t text_vaddr = shdr->sh_addr;
                    uintptr_t text_phys = arch_translate(task.pagemap, text_vaddr);
                    debug(true, "Copying .text to 0x%lx\n", text_phys);
                    memcpy((void *)text_phys, (const void *)(elf + shdr->sh_offset), shdr->sh_size);
                }

                if (!strncmp(&section_names[shdr->sh_name], ".rodata", 7))
                {
                    debug(true, "Found .rodata section\n");
                    uintptr_t rodata_vaddr = shdr->sh_addr;
                    uintptr_t rodata_phys = arch_translate(task.pagemap, rodata_vaddr);
                    debug(true, "Copying .rodata to 0x%lx\n", rodata_phys);
                    memcpy((void *)rodata_phys, (const void *)(elf + shdr->sh_offset), shdr->sh_size);
                }
            }
        }
        phdr = (Elf64_Phdr *)((uint8_t *)phdr + ehdr.e_phentsize);
    }

    if (!found_ptload)
    {
        panic_if(panic_on_fail, "Could not find a PT_LOAD segment!");
        return (Task){};
    }

    // Stack memory (8KiB)
    size_t stack_size = 2 * PAGE_SIZE; // 8KiB
    task.ustack = ehdr.e_entry + PAGE_SIZE; // Adjust to ensure non-overlap
    size_t stack_top = task.ustack + stack_size;

    for (size_t i = 0; i <= stack_size / PAGE_SIZE; i++) {
        auto addr = task.ustack + i * PAGE_SIZE;
        arch_map_page(task.pagemap, addr, addr, MAP_USER_RW);
    }

    // Ensure rsp points to the top of the stack
    task.ustack = task.registers.registers.rbp = task.registers.rsp = stack_top;
    return task;
}
