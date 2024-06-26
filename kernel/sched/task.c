#include "task.h"
#include <mm/virt.h>

Task create_task_struct(string_view descriptor, Elf64_Addr entrypoint)
{
	return (Task){
		.descriptor = descriptor,
		.pagemap = arch_create_new_pagemap(),
		.entrypoint = entrypoint,
		.registers.ip = entrypoint,
		.ustack = 0,
		.registers.registers = (GeneralRegisters){}
	};
}