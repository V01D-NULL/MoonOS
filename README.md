# VALIDITY OS
Validity OS is a monolithic kernel targeting the x86_64 architecture.

I will attempt to make it POSIX compliant

# Screenshots:
<img src="screenshot/1.png">

# Features
- Limine bootloader
- CPU Identification
- Vesa with font loading using ssfn2
- Support for 4 and 5 level paging

# Future features:
- A simple heap
- APIC
- ACPI
- Userland support
- Multitasking
- SMP
- Many hardware drivers including but not limited to:
	- Network cards
	- ATA
	- GPU's
	- And much more
- A libc for the userland

# This is currently being working on:
- vmm cleanup
- Parsing ACPI tables

# Directory walkthrough:
- libs/   	  --  Here you will find kernel libs and ports. (Planning on putting things like bash and Doom in this lib/ folder)
- kernel/ 	  --  Kernel source code, this is most likely all you really care about
- build-util/ --  3rd party tools needed to build the Validity kernel + makefile flags
- debug-util/ --  Files needed for debugging Validity.
- boot/		  --  Everything limine needs to boot 
- font/		  --  Contains the sfn font file which will be used by the kernel once VESA is enabled

# Installation
## Building: (assuming you are using a debian based distro)
- Install needed tools:
	- sudo apt update
	- sudo apt install build-essential nasm qemu-system-x86
- Build kernel
	- git clone < REPO >
	- cd < REPO_DIR >
	- make all
	- make run

## Building an ISO
- Run `make`, that's literally all there is to it, lol :)

# My journey: (Inspiration for newcomers)
I have always wanted to make an OS, so one day I decided to build one.
Turns out it is alot of work and back then it totally overwhelmed me to the point where I took long breaks (for months) because information was so scarce, and what little informate there is is outdated.

I have written about 4 "kernels" during my OS development learning experience, and each time I started over because I was far too overwhelmed- none have gotting past a GDT :/   (heck, some where just bootloaders that called a kernel entry and looped)

If you are a beginner looking to understand what this code is doing and how you can write your own kernel with up-to-date information, please do join the OSDEV discord server (linked in the osdev wiki at the bottom) and don't give up when it gets hard at first.

I have started developing kernels about 8 months ago (with multiple month-long of breaks inbetween), and I still have many things to learn, however my `try harder` mindset proved useful in helping me get through the struggles and write a kernel for which I did not have to steal or copy code from tutorials or other projects out of a lack of knowledge.

I hope this inspired you to use this mindset when things seem too difficult to manage.

Best of luck ~ V01D aka Tim.

# Special thanks
- Thanks go out to websites like lowlevel.eu or the osdev wiki but also some cool and really helpful os developers on github and discord.

# Finishing touch (resources I use)
* https://www.cs.cmu.edu/~ralf/files.html
* https://ethv.net/workshops/osdev/notes/notes-3.html
* https://osdev.wiki  (barebones but up-to-date osdev wiki)
* https://wiki.osdev.org
