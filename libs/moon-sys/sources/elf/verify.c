#include "moon-sys/elf/verify.h"

Elf64_Ehdr elf_verify_ehdr(const uint8_t *elf)
{
	Elf64_Ehdr ehdr = *(Elf64_Ehdr *)elf;

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
