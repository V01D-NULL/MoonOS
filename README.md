# VALIDITY OS
Validity OS is a monolithic kernel targeting the x86_64 architecture.

# Where did everything go?
As you might have noticed there are alot less files in this repo than there were before. This is because I have decided to switch from GRUB to limine, and from protected to long mode, so most of the protected mode code had to go.
- Why switch?
	- **Seemingly endless address space** (Do I really need to say more after that? :D ), XSDT, APIC (besides, protected mode is super obsolete. (Technically this OS is obsolete aswell since it isn't UEFI but eh. (Jokes on you I can see my private Validty-UEFI repo that I will work on eventually which will be an exact clone of this kernel but for UEFI systems) It's still good practice and doesn't differ from UEFI that much (I think)))

# Features
- Stivale2 boot protocol
- Limine bootloader
- GDT (Credits to https://github.com/ethan4984/rock/ for the gdt code (The only change I made was to add a null descriptor to the GDT))
- Interrupts
- CPU Identification
- Serial output

# Future features:
- A simple heap
- A memory allocator which acts as a wrapper for vmm and pmm (haven't decided on the type yet (slab, buddy, bitmap, linked list, etc))
- Paging
- APIC
- ACPI
- Many hardware drivers including but not limited to:
	- Network cards
	- ATA
	- GPU's
	- And much more
- Simple userland mode
- A small libc for the userland

# This is currently being working on:
- Creating a libs/ folder in the projects root directory where .a files will be generated for use by the Validity kernel (files will be those in the kernel/libk/ folder and more will be added as time goes on)
- pmm

# Installation
## Building:
- ### For devs (assuming you are using a debian based linux distro)
	- Install needed tools:
		- sudo apt update
		- sudo apt install build-essential nasm qemu-system-x86
	- Build kernel
		- git clone < REPO >
		- cd < REPO_DIR >
		- make all
		- make run

# Resources:
- This kernel started by me learning from james JamesM's kernel development tutorials, however I changed all of his code because of the bad and partially outdated implementations he presented. Thanks go out to websites like lowlevel.eu or the osdev wiki but also some cool and really helpful os developers on github and discord.

* https://www.cs.cmu.edu/~ralf/files.html
* https://ethv.net/workshops/osdev/notes/notes-3.html
* https://osdev.wiki  (barebones but up-to-date osdev wiki)
* https://wiki.osdev.org
