/*  _  _   __   __    __  ____  __  ____  _  _     __   ____ 
   / )( \ / _\ (  )  (  )(    \(  )(_  _)( \/ )   /  \ / ___)
   \ \/ //    \/ (_/\ )(  ) D ( )(   )(   )  /   (  O )\___ \
    \__/ \_/\_/\____/(__)(____/(__) (__) (__/     \__/ (____/
    =========================================================
	* Project name: Validity OS								*
	* Author: Tim aka. V01D (timstert23@gmail.com)   			*
	=========================================================

*/

// CPU/Architecture specific
#include "arch/x86/cpu/descriptor_tables/descriptorTables.h"
#include "arch/x86/cpu/interrupts/interrupts.h"
#include "arch/x86/cpuid.h"

// HAL
#include "common.h"  //I will let this slide as HAL because of it's in / out functions
#include "hal/pit.h"
#include "hal/cmos.h"
#include "hal/pci.h"

// Drivers
#include "drivers/keyboard/keyboard.h"
#include "drivers/screen/monitor.h"
#include "drivers/io/serial.h"
#include "drivers/screen/cursor.h"
#include "drivers/mouse/mouse.h"

//Mem
#include "mem/heap.h"
#include "mem/paging.h"

//VFS
#include "fs/vfs.h"
#include "fs/pre_env/initrd.h"

// Misc
#include "multiboot.h"
#include "sys/timer.h"
#include "panic.h"
#include "ascii_art.h"

//For some reason everything inside of sys/timer.h has to be re-defined. Otherwise it will be an implicit declaration. IDK why, if you do- please tell me.
//Also note that this only occurs inside of this main.c file. Every other file is fine.
void install_pit();
void sleep(u32int seconds);
void beep(u32int freq);
/*
	TODO:
		- Create a crypto/weak/ceaser.c file which will use the ceaser cipher
		- Read this and enable scrolling up & down: http://www.petesqbsite.com/sections/tutorials/tutorials/3scroll.htm
*/

/*
	Resources:
		Memory:
			- https://forum.osdev.org/viewtopic.php?f=15&t=23982
			- https://wiki.osdev.org/Brendan%27s_Memory_Management_Guide
			- https://wiki.osdev.org/Memory_management
			
*/


void kernel_demo_end() {
	kill_cursor();
	unregister_interrupt_hanlder(IRQ0);
	monitor_write("System ready -- Demo complete.\n", false, true);
	monitor_write("Check in another time to see if any updates to the github repo have occured", false, true);
	monitor_write("If you enjoyed it, feel free to leave a star and share the word ^^\n", false, true);
	while(1)
		get_date();
}


void get_boot_info(struct multiboot* mboot_ptr)
{
	//TODO:
	// Don't be lazy and actually check if this info is available aka if the bit at whatever index is set.
	// Check the OSDEV Wiki for more
	debug("***************************\nMultiboot info:\n");
	debug("> Bootloader name: %s\n", mboot_ptr->boot_loader_name);
	debug("> Boot device: %x\n", mboot_ptr->boot_device);
	debug("> Cmdline: %s\n", mboot_ptr->cmdline);
	debug("***************************\n");
}


void init (struct multiboot *mboot_ptr)
{
	//Debug output to stdout will be white by default
	serial_set_color(BASH_WHITE);

	enable_cursor(12, 12); //Set custom cursor. It's sleeker :)

	// Initialize screen (by clearing it)
	monitor_clear();

	debug("[+] STARTED VALIDITY OS [+]\n%s %s\n\n", __DATE__, __TIME__);
	monitor_write("Loaded kernel\n", false, true);
	monitor_write("Welcome to:\n", false, true);
	banner();

	//Get multiboot info
	get_boot_info(mboot_ptr);

	debug(".text: %x\n.data: %x\n.bss: %x\n", get_section_text(), get_section_data(), get_section_bss());
	debug("***************************\n");
	debug("Physical memory: %x\n", get_physical_memory_map(mboot_ptr));

	//Check the CPU
	checkCPU();
	
	// Initialize ISRs and segments
	init_descriptor_tables();

	//Install PIT IRQ
	install_pit();
	
	//Install keyboard IRQ
	install_keyboard();

	//Init cmos / rtc
	init_cmos();
}


// void trigger_page_fault() { u32int *ptr = ( u32int* ) 0xA0000000; u32int trigger_page_fault = *ptr; }


int kmain ( struct multiboot *mboot_ptr )
{
	init(mboot_ptr);	

	init_vfs(mboot_ptr);
	
	// init_pci();
	
	// list_fs();
	// list_file("/ValidityOS.txt");
	// kernel_demo_end();

	return 0;
}
