# VALIDITY OS
Validity OS is a monolithic kernel targeting the x86_64 architecture.

# Features
- Stivale2 boot protocol
- Limine bootloader
- GDT
- Interrupts (kinda, custom IRQ's don't get called and cause a GPF if called using the int instruction)
- CPU Identification
- Serial in & output

# Future features:
- A simple heap / memory allocator
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
		- make all
		- make run

# Resources:
- This kernel started by me learning from james JamesM's kernel development tutorials, however I changed almost all of his code because of the bad and partially outdated implementations he presented. Thanks go out to websites like lowlevel.eu or the osdev wiki but also some cool and really helpful os developers on github and discord.

* https://rwth-os.github.io/eduOS/
* https://github.com/kAworu/JamesM
* https://github.com/cirosantilli/jamesmolloy-kernel-development-tutorials
* https://github.com/berkus/jamesm-tutorials/
* https://github.com/Usemox/Usemox/
* https://www.cs.cmu.edu/~ralf/files.html
