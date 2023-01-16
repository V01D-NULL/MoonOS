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
	Elf64_Half phdr_entries = ehdr.e_phnum;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr.e_phoff);

	Task task = create_task_struct(descriptor, ehdr.e_entry);

	size_t elf_mapping_offset = phdr->p_vaddr;
	bool found_ptload = false;
	arch_copy_kernel_mappings(task);

			auto phys = arch_alloc_page();
	for (Elf64_Half i = 0; i < phdr_entries; i++)
	{
		if (phdr->p_type == PT_LOAD)
		{
			found_ptload = true;
			size_t num_pages = ALIGN_UP(phdr->p_memsz) / 4096;


			for (uint64_t i = 0; i < num_pages; i++)
			{
				elf_mapping_offset += (i * PAGE_SIZE);
				arch_quickmap_page(task.pagemap, elf_mapping_offset, (size_t)phys, MAP_USER_RW);
			}

			memcpy((uint8_t *)phys, (const uint8_t *)elf + phdr->p_offset, phdr->p_filesz);
			memset((void *)phys + phdr->p_filesz, 0, phdr->p_filesz - phdr->p_memsz);
			
			// Elf64_Shdr *shstrtab = elf + (ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize);
			// string_view section_names = elf + shstrtab->sh_offset;
// 
			// for (int i = 0; i < ehdr.e_shnum; i++)
		// 	{
		// 		Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + (ehdr.e_shoff + i * ehdr.e_shentsize));

		// 		if (!strncmp(&section_names[shdr->sh_name], ".rodata", 7))
		// 		{
		// 			memcpy((uint8_t *)(phys + shdr->sh_offset), (uint8_t *)(elf + shdr->sh_offset), ehdr.e_shentsize);
		// 		}
		// 	}
		}

		phdr += ehdr.e_phentsize;
	}
	
	// elf_load_section((void *)elf, ".data", ehdr);

	// auto base = arch_alloc_page();
	// arch_quickmap_page(task.pagemap, shdr->sh_addr, arch_alloc_page(), MAP_USER_RO);
	// elf_load_section((void *)elf, ".rodata", ehdr, task.pagemap);

	if (!found_ptload)
	{
		panic_if(panic_on_fail, "Could not find a PT_LOAD segment!");
		return (Task){};
	}

	// Stack memory (8KiB)
	task.ustack = elf_mapping_offset + PAGE_SIZE;
	for (size_t i = 1; i <= 2; i++)
	{
		auto addr = elf_mapping_offset = i * PAGE_SIZE + task.ustack;
		arch_map_page(task.pagemap, addr, addr, MAP_USER_RW);
	}
	// Todo: Shadow stack page

	return task;
}