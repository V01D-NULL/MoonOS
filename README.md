# VALIDITY OS
Validity OS is a monolithic kernel targeting the x86 architecture (IA32 ~ 32 bits).

# Features
- Mulitboot compliant
	- Uses GRUB2
- GDT
- Interrupts
- CPU Identification
- Serial in & output
- CMOS/RTC Timestamp
- Simple VFS

# Future features:
- A simple heap / memory allocator
- Paging
- Multiboot2 (Due to my computer's problems this is on hold for an unkown timespan (that is until I can afford a new computer))
- ACPI
- Many hardware drivers including but not limited to:
	- Network cards
	- ATA
	- GPU's
	- And much more
- Simple userland mode
- A small libc for the userland

# This is currently being working on:
- A rewrite of the allocation and paging implementations (I kinda stole the code from a tutorial and wish to rewrite it in order to increase the level of independence for me, my learning experience and my kernel)

# Installation
## Building:
- ### For devs (assuming you are using a debian based linux distro)
	- Install needed tools:
		- sudo apt update
		- sudo apt install build-essential nasm qemu-system-x86
	- Build kernel
		- git clone < REPO >
		- cd < REPO_DIR >
		- make qemu
	- Debug kernel:
		- make qemu-gdb

	- If any of the compilation steps gives you an error, then you most likely don't have any object files the makefile tries to remove. If this is the case, execute make _all and try to compile again or type make run from there on since make _all compiles everything, but doesn't run the kernel.

# Resources:
-  This kernel is mainly built upon JamesM's kernel development tutorials, however many hours of googling and other resources have brought me to this point.

* https://rwth-os.github.io/eduOS/
* https://github.com/kAworu/JamesM
* https://github.com/cirosantilli/jamesmolloy-kernel-development-tutorials
* https://github.com/berkus/jamesm-tutorials/
* https://github.com/Usemox/Usemox/
* https://www.cs.cmu.edu/~ralf/files.html
