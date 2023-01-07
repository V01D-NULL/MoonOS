#include "elf.h"
#include <panic.h>
#include <printk.h>
#include <moon-io/serial.h>
#include <mm/virt.h>
#include <mm/phys.h>
#include <base/base-types.h>
#include <base/string.h>
#include <base/align.h>

static Elf64_Ehdr elf_verify_ehdr(const uint8_t *elf);
static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, struct elf_loader_args args);

Task load_elf(const uint8_t *elf, struct elf_loader_args args)
{
	Elf64_Ehdr ehdr = elf_verify_ehdr(elf);
	return elf_parse_phdr(elf, ehdr, args);
}

static Elf64_Ehdr elf_verify_ehdr(const uint8_t *elf)
{
	Elf64_Ehdr ehdr = *(Elf64_Ehdr*)(elf);

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

static Task elf_parse_phdr(const uint8_t *elf, Elf64_Ehdr ehdr, struct elf_loader_args args)
{
	Elf64_Half phdr_entries = ehdr.e_phnum;
	Elf64_Phdr *phdr = (Elf64_Phdr *)((uintptr_t)(elf) + ehdr.e_phoff);
	Task task = create_task_struct(args.descriptor, ehdr.e_entry);

	size_t elf_mapping_offset = phdr->p_vaddr;
	bool found_ptload = false;
	arch_copy_kernel_mappings(task);

	for (int i = 0x400000; i <= 0x404000; i += 4096)
		arch_map_page(task.pagemap, i, i, MAP_USER_RW);

	// 0xffffffff802196b8

	// arch_map_page(task.pagemap, 0, 0, MAP_USER_RW);
	for (Elf64_Half i = 0; i < phdr_entries; i++)
	{
		if (phdr->p_type == PT_LOAD)
			{
		// 		found_ptload = true;
		// 		size_t num_pages = ALIGN_UP(phdr->p_memsz) / 4096;

		// 		// printk("elf", "Virtual mapping: 0x%lX (%d bytes | %d pages)\n", phdr->p_vaddr, phdr->p_memsz, num_pages);

		// 		for (uint64_t i = 0; i < num_pages; i++)
		// 		{
		// 			elf_mapping_offset += (i * PAGE_SIZE);
		// 			arch_map_page(task.pagemap, elf_mapping_offset, elf_mapping_offset, MAP_USER_RW);
		// 		}
		trace("Copying 0x%lx to 0x%lx (size: %ld, base: 0x%lx)\n", elf + phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, elf);
		memcpy((uint8_t *)phdr->p_vaddr, elf + phdr->p_offset, phdr->p_filesz);
		// break;
		// for(;;);
		memset((void *)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_filesz - phdr->p_memsz);
	}

		phdr += ehdr.e_phentsize;
	}

	// if (!found_ptload)
	// {
	// 	panic_if(args.do_panic, "Could not find a PT_LOAD segment!");
	// 	return (Task){};
	// }

	// // Stack memory (8KiB)
	// task.ustack = elf_mapping_offset + PAGE_SIZE;
	// for (size_t i = 1; i <= 2; i++)
	// {
	// 	auto addr = elf_mapping_offset = i * PAGE_SIZE + task.ustack;
	// 	arch_map_page(task.pagemap, addr, addr, MAP_USER_RW);
	// }
	// Todo: Shadow stack page

	return task;
}
