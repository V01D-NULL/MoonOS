### Todo list

IMPORTANT: Remember to update the README and resume if this works out.

- Move the syscall implementation to `arch/some_architecture`, then create a bunch of system calls like map and unmap.
- Create `arch_sleep()` in `arch/some_architecture` and call it arch/something/time/sleep.c (It will replace kernel/time/*)
- Make kernel/time/ an archive library in libs/
- Make arch/x86/smp/spinlock/ an archive library in libs/
- Create a generic HandoverMap to get rid of the stivale2 memory map for portability reasons.
- Move a lot of stuff around and generalize it (Example):
	- vmm.c should call `arch_map_page`, `arch_quickmap_page`, etc instead of implementing paging.
	- pmm.c should use a `struct HandoverMap` instead of a stivale2 memory map.

- Rename BootContext to BootHandover
- Replace `struct stivale2_struct_tag_modules *mods` with something like `struct HandoverModules *`. If used with limine it just contains pointers to the beginning of each module and their sizes. If used with something that doesn't have a bootloader like the 3ds, read the modules from the SD car before passing control to `kern_main`. panic() if that goes wrong.

### Misc
- Move .gitignore, .gitmodules, Logo.jpeg, bochsrc and font.bin into misc/. Update make and markdown files accordingly

When it comes to merging, I guess just search for `git forcibly merge` online and do that.