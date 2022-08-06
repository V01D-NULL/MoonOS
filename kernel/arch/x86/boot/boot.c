#include <devices/term/limine-port/term.h>
#include <devices/term/early/early_term.h>
#include <boot/proto/proto-stivale2.h>
#include <moon-io/serial.h>
#include <base/mem.h>
#include <base/bootargs.h>
#include <arch/x86/hardware/legacy/pic/pic.h>
#include <moon.h>
#include <arch/x86/int/idt.h>
#include <arch/x86/int/gdt.h>
#include <kernel.h>
#include <mm/dynamic/kmalloc.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <printk.h>
#include "stivale2.h"

const char serial_message[] = {
	"███╗   ███╗ ██████╗  ██████╗ ███╗   ██╗ ██████╗ ███████╗ \n"
	"████╗ ████║██╔═══██╗██╔═══██╗████╗  ██║██╔═══██╗██╔════╝ \n"
	"██╔████╔██║██║   ██║██║   ██║██╔██╗ ██║██║   ██║███████╗ \n"
	"██║╚██╔╝██║██║   ██║██║   ██║██║╚██╗██║██║   ██║╚════██║ \n"
	"██║ ╚═╝ ██║╚██████╔╝╚██████╔╝██║ ╚████║╚██████╔╝███████║ \n"
	"╚═╝     ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚══════╝ \n"};

const char fb_message[] = {
	"\n"
	" _____             _____ _____  \n"
	"|     |___ ___ ___|     |   __| \n"
	"| | | | . | . |   |  |  |__   | \n"
	"|_|_|_|___|___|_|_|_____|_____| \n"
	"\n"};

void banner(bool serial_only)
{
	if (serial_only)
	{
		debug(false, "%s", serial_message);
		return;
	}

	puts("Welcome to MoonOS\n");
	puts("%s", fb_message);
}

void boot_fail(string_view reason)
{
	debug(false, "%s", reason);
	boot_term_write("%s", reason);
	for (;;)
		asm volatile("cli;hlt");
}

EXTERNAL(stack);

void boot(struct stivale2_struct *bootloader_info)
{
	BootHandover ctx = {};

	struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	struct stivale2_struct_tag_terminal *term = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_TERMINAL_ID);
	struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	struct stivale2_struct_tag_cmdline *cmdline = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_CMDLINE_ID);
	struct stivale2_struct_tag_modules *modules = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MODULES_ID);

	if (!term)
		set_boot_term_available(false);
	else
		boot_term_init(term->term_write, term->cols, term->rows);

	// Can't work under these conditions
	if (!fb)
		boot_fail(BASH_RED "WARNING: " BASH_DEFAULT "MoonOS was NOT provided with a framebuffer. Refusing to boot.");

	if (rsdp != NULL)
		ctx.rsdp = rsdp->rsdp;

	if (mmap != NULL)
	{
		// Prepare the terminal
		boot_term_write("boot: Reached target term_prepare\n");
		term_prepare(fb, mmap);

		boot_term_write("boot: Reached target pmm\n");
		{
			extern void init_phys_allocator(void *, int);
			init_phys_allocator(mmap->memmap, mmap->entries);
		}

		boot_term_write("boot: Reached target vmm\n");
		{
			extern void arch_init_paging(void);
			arch_init_paging();
		}		

		boot_term_write("boot: Reached target gdt and tss\n");
		init_gdt((uint64_t)stack + sizeof((uint64_t)stack)); // Note: boot_term_write is unusable now

		// Core#0 will remap the pic once.
		// After acpi_init the pic is disabled in favor of the apic
		pic_remap();
		init_idt();

		/* Is verbose boot specified in the command line? */
		if (cmdline != NULL)
		{
			auto verbose = bootarg_find("verbose_boot", (string_view)cmdline->cmdline);
			printk_init(verbose);
		}
		else
		{
			boot_fail("No boot cmdline\n");
		}

		banner(false);
	}
	else
		boot_fail("Fatal: Cannot obtain a memory map from the bootloader");

	kalloc_init();
	init_percpu((uint64_t)stack);
	kern_main(&ctx, modules);
}