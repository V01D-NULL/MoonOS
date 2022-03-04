/**
 * @file bootloader_stivale2.c
 * @author Tim (V01D)
 * @brief Handles information from the bootloader
 * @version 0.1
 * @date 2021-04-15
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "bootloader_stivale2.h"
#include <devices/serial/serial.h>
#include <amd64/moon.h>
#include <stivale2.h>
#include <int/idt.h>
#include <int/gdt.h>
#include <kernel.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/mm.h>
#include <panic.h>
#include <stdint.h>
#include <printk.h>
#include <libgraphics/double-buffering.h>
#include <libgraphics/draw.h>
#include <libgraphics/bootsplash.h>
#include <drivers/keyboard/keyboard.h>
#include <hal/pic/pic.h>
#include <devices/term/fallback/fterm.h>
#include <devices/fb/early_fb.h>
#include <libk/cmdline.h>
#include <libgraphics/bootsplash_img.h>
#include <mm/slab.h>
#include <mm/buddy/buddy.h>

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

static gnu_section_align16 uint8_t stack[8192];

struct stivale2_struct_tag_cmdline cmdline_tag = {
	.tag = {
		.identifier = STIVALE2_STRUCT_TAG_CMDLINE_ID,
		.next = 0}};

struct stivale2_struct_tag_rsdp rsdp_tag = {
	.tag = {
		.identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
		.next = (uintptr_t)&cmdline_tag}};

struct stivale2_tag level5_paging_tag = {
	.identifier = STIVALE2_HEADER_TAG_5LV_PAGING_ID,
	.next = (uintptr_t)&rsdp_tag};

struct stivale2_header_tag_smp smp_hdr_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_SMP_ID,
		.next = (uintptr_t)&level5_paging_tag}};

struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
	// All tags need to begin with an identifier and a pointer to the next tag.
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		.next = (uintptr_t)&smp_hdr_tag},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 32};

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
		.next = (uintptr_t)&framebuffer_hdr_tag},
	.flags = 0};

SECTION(".stivale2hdr")
struct stivale2_header stivale_hdr = {
	.entry_point = 0,
	.stack = (uintptr_t)stack + sizeof(stack),
	.flags = (1 << 2) | (1 << 4),
	.tags = (uintptr_t)&terminal_hdr_tag};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
	struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
	for (;;)
	{

		if (current_tag == NULL)
		{
			return NULL;
		}

		if (current_tag->identifier == id)
		{
			return current_tag;
		}

		// Get a pointer to the next tag in the linked list and repeat.
		current_tag = (void *)current_tag->next;
	}
}

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

	printk("main", "Welcome to MoonOS\n");
	printk("banner", "%s", fb_message);
	delay(200);
}

/**
 * @brief Kernel entry point
 *
 * @param[in] bootloader_info Various information from the limine bootloader
 */
void kinit(struct stivale2_struct *bootloader_info)
{
	boot_info_t bootvars;
	struct stivale2_struct_tag_terminal *term = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_TERMINAL_ID);
	struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
	struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	struct stivale2_struct_tag_cmdline *cmdline = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_CMDLINE_ID);
	struct stivale2_struct_tag_modules *modules = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MODULES_ID);
	struct stivale2_struct_tag_firmware *fw = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FIRMWARE_ID);
	bootvars.is_uefi = !(fw->flags & STIVALE2_FIRMWARE_BIOS);

	if (!term)
	{
		set_fterm_available(false);
	}
	else
	{
		fterm_init(term->term_write, term->cols, term->rows);
	}

	if (fb != NULL)
	{
		bootvars.vesa.fb_addr = fb->framebuffer_addr;
		bootvars.vesa.fb_width = fb->framebuffer_width;
		bootvars.vesa.fb_height = fb->framebuffer_height;
		bootvars.vesa.fb_bpp = fb->framebuffer_bpp;
		bootvars.vesa.fb_pitch = fb->framebuffer_pitch;
	}
	else
	{
		for (;;)
			;
	}

	if (mmap != NULL)
	{
		fterm_write("boot: Reached target gdt and tss\n");
		init_gdt((uint64_t)stack + sizeof(stack));

		// Core#0 will remap the pic once.
		// After acpi_init the pic is disabled in favor of the apic
		fterm_write("boot: Reached target pic and idt\n");
		pic_remap();
		init_idt();

		fterm_write("boot: Reached target pmm\n");
		pmm_init(mmap->memmap, mmap->entries);

		fterm_write("boot: Reached target slab\n");
		slab_init();

		fterm_write("boot: Buddy init..\n");
		buddy_init(mmap->memmap, mmap->entries);
		fterm_write("boot: After buddy init\n");

		// TEST //
		struct page *page  = buddy_alloc(0);
		struct page *page1 = buddy_alloc(PAGE);
		struct page *page2 = buddy_alloc(PAGE);

		if (page != NULL)
			fterm_write("1. buddy_alloc(0) => { order: %d, ptr: 0x%llX }\n", page->order, page->ptr);
		else
			fterm_write("1. (Allocation Failure) buddy_alloc(0) => { order: N/A, ptr: NULL }\n");

		if (page1 != NULL)
			fterm_write("2. buddy_alloc(PAGE) => { order: %d, ptr: 0x%llX }\n", page1->order, page1->ptr);
		else
			fterm_write("2. (Allocation Failure) buddy_alloc(PAGE) => { order: N/A, ptr: NULL }\n");

		if (page2 != NULL)
			fterm_write("3. buddy_alloc(PAGE) => { order: %d, ptr: 0x%llX }\n", page2->order, page2->ptr);
		else
			fterm_write("3. (Allocation Failure) buddy_alloc(PAGE) => { order: N/A, ptr: NULL }\n");

		// foo();

		for (;;)
			;
		// TEST //

		fterm_write("boot: Reached target vmm\n");
		vmm_init(check_la57(), mmap);

		/* Is verbose boot specified in the command line? */
		if (cmdline != NULL)
		{
			bool vm_tag_present = boot_cmdline_find_tag("vm", (const char *)cmdline->cmdline);

			if (!boot_cmdline_find_tag("verbose_boot", (const char *)cmdline->cmdline))
			{
				fterm_write("boot: Quiet boot flag set\n");
				fterm_flush();
				early_fb_init(bootvars);
				fb_draw_image((bootvars.vesa.fb_width / 2) - (IMG_WIDTH / 2), (bootvars.vesa.fb_height / 2) - (IMG_HEIGHT / 2), IMG_WIDTH, IMG_HEIGHT, IMG_DATA, IMAGE_RGB);
				printk_init(false, bootvars, vm_tag_present);
			}
			else
			{
				fterm_write("boot: Verbose boot flag set\n");
				fterm_flush();
				printk_init(true, bootvars, vm_tag_present);
			}
		}
		else
		{
			for (;;)
				;
		}

		banner(false);
		printk("pmm", "Initialized pmm\n");

		/* vmm */
		if (check_la57())
		{
			printk("vmm", "pml5 resides at 0x%llx\n", cr_read(CR3));
		}
		else
		{
			printk("vmm", "pml4 resides at 0x%llx\n", cr_read(CR3));
		}
		printk("vmm", "Initialized vmm\n");
	}
	else
	{
		debug(false, "\n!Did not get a memory map from the bootloader!\n");
		fterm_write("Fatal: Cannot obtain a memory map from the bootloader");
		for (;;)
			;
	}

	if (smp != NULL)
	{
		bootvars.cpu.processor_count = smp->cpu_count;
		bootvars.cpu.bootstrap_processor_lapic_id = smp->bsp_lapic_id;
		bootvars.cpu.acpi_processor_uid = smp->smp_info->processor_id;
		bootvars.cpu.lapic_id = smp->smp_info->lapic_id;
		bootvars.cpu.smp_info = smp->smp_info;
	}

	if (rsdp != NULL)
	{
		bootvars.rsdp.rsdp_address = rsdp->rsdp;
	}

	log_cpuid_results();
	kmain(&bootvars, modules);
}
