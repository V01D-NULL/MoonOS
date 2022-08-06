#ifndef BOOT_INFO_HANDOVER_H
#define BOOT_INFO_HANDOVER_H

#include <base/base-types.h>

// clang-format off
struct$(BootHandover,
{
#if defined(__x86_64__) || defined(__aarch64__)
	uint64_t rsdp;
#endif
});
// clang-format on

struct HandoverModules
{
	struct
	{
		uint8_t *module_ptr;
		uint64_t module_size;
	} modules[16];
};

#endif // BOOT_INFO_HANDOVER_H