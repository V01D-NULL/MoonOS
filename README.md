# Moon OS

<h2 align="center"> Moon OS is a modular micro-kernel targeting the x86_64 and armv6 architecture, aiming to be POSIX-like in the future. </h2>
<p align="center">
<img align="center" src="meta/Logo 500x500.jpeg">
</p>


# Screenshots:
Bootsplash:
<img src="meta/screenshot/bootsplash.png">
<br>
Verbose boot:
<img src="meta/screenshot/verbose_boot.png">
<br>
Kernel panic:
<img src="meta/screenshot/kpanic.png">
<br>

# Features
- Support for multiple architectures:
	- x86_64 PCs with acpi support
		- 4 level paging
		- Stacktrace/symbol backtrace
		- zoned bitmap and slab allocator.
		- System calls via `syscall` (No support for `int` based system calls)
	- Nintendo 3ds (armv6)
		- I2C driver
		- Bootrom IVT
		- Render strings to the display

# Shared features (both architectures support this):
- ubsan
- Parsable boot arguments (quiet/verbose)
- Elf loader
- Primitive scheduler (Will be improved overtime, for now the my focus is on getting everything up and running (IPC, servers, libc, etc))

# Future features:
- Threads
- kasan
- ACPI
- SMBIOS
- Multitasking
- Many hardware drivers including but not limited to:
	- Network cards
	- Storage drivers
	- GPU's
	- And much more
- A libc for the userland

# Near future todo list
- Make better use of the APIC (currently it's used for the timer only)
- virtual memory subsystem
- New physical memory manager? (Unsure.)

# This is currently being working on:
- Scheduler
- IPC
- Porting mlibc
- Initrd
	- In kernel decompression library to decompress the initrd
	- Native (build machine) program to build and add files (to maintain decent modularity) to the initrd.
	- Load and render an image from the initrd (tga, bmp)
- New additions to make the armv6 port a lot more usable (SD Card driver, gpu, vblank)

# Directory walkthrough:
- libs/   	  --  Utilities not strictly related to a kernel (Scheduler, memory manager, etc), but required for it to function.
- kernel/ 	  --  Kernel source code
- boot/		  --  Everything Moon needs in order to boot 
- daemon/     --  Special micro-kernel servers
- userspace/  --  Bootloader files, configs, modules, etc. Many files are automatically added at build-time.

# Installation
## Building: (assuming you are using a debian based distro)
- Install needed tools:
	- sudo apt update
	- sudo apt install gcc nasm qemu-system-x86 libfuse-dev
- Build kernel
	- `make all` -- Build the kernel
	- `make run` -- Run the kernel in qemu and build it if necessary (x86_64 only)
	- `make kvm` -- Run the kernel in qemu with kvm and build it if necessary (x86_64 only)
	- Command line options for `kvm` and `run`:
		- `modern=yes` -- Emulate modern features (x86_64 only)

- Configure architecture:
	- Simply edit two variables in `Make.conf`:
		- `ARCH` and `BOARD`. Permitted values are documented in a comment.


# My journey: (Inspiration for newcomers)
I have always wanted to make an OS, so one day I decided to build one.
Turns out it is a lot of work and back then it totally overwhelmed me to the point where I took long breaks (for months) because information was so scarce, and what little information there is is outdated.

I have written about 4 "kernels" during my OS development learning experience, and each time I started over because I was far too overwhelmed- none have gotten past a GDT :/   (heck, some where just bootloaders that called a kernel entry and looped)

If you are a beginner looking to understand what this code is doing and how you can write your own kernel with up-to-date information, please do join the OSDEV discord server (linked in the osdev wiki at the bottom) and don't give up when it gets hard at first.

I have started developing kernels about 8 months ago at the time of writing (with multiple month-long of breaks in between), and I still have many things to learn, however my `try harder` mindset proved useful in helping me get through the struggles and write a kernel for which I did not have to steal or copy code from tutorials or other projects out of a lack of knowledge.

I hope this inspired you to use this mindset when things seem too difficult to manage.

Best of luck ~ V01D (Tim).

# Special thanks
- Thanks go out to websites like lowlevel.eu or the osdev wiki but also some cool and really helpful os developers on github and discord.

# Finishing touch (resources I use)
* https://www.cs.cmu.edu/~ralf/files.html
* https://ethv.net/workshops/osdev/notes/notes-3.html
* https://osdev.wiki  (barebones but up-to-date osdev wiki)
* https://wiki.osdev.org
