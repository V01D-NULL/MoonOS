#include "moon-sys/elf/load_section.h"
#include <base/string.h>
#include <mm/virt.h>
#include <paging/paging.h>

bool elf_load_section(void *elf, string_view target, Elf64_Ehdr ehdr, void *base)
{
	size_t target_len = strlen(target);
	Elf64_Shdr *shstrtab = elf + (ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize);
	string_view section_names = elf + shstrtab->sh_offset;

	for (int i = 0; i < ehdr.e_shnum; i++)
	{
		Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + (ehdr.e_shoff + i * ehdr.e_shentsize));

		if (!strncmp(&section_names[shdr->sh_name], target, target_len))
		{
			auto phys = arch_alloc_page();
			arch_quickmap_page((struct Pml*)base, shdr->sh_addr, (size_t)phys, MAP_USER_RO);
			memcpy((uint8_t *)phys, (uint8_t *)(elf + shdr->sh_offset), ehdr.e_shentsize);
			return true;
		}
	}

	return false;
}
