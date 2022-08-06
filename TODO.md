### Todo list

IMPORTANT: Remember to update the README and resume if this works out.

- Move a lot of stuff around and generalize it (Example):
	- vmm.c should call `arch_map_page`, `arch_quickmap_page`, etc instead of implementing paging.

- Replace `struct stivale2_struct_tag_modules *mods` with something like `struct HandoverModules *`. If used with limine it just contains pointers to the beginning of each module and their sizes. If used with something that doesn't have a bootloader like the 3ds, read the modules from the SD car before passing control to `kern_main`. panic() if that goes wrong.

### Misc
When it comes to merging, I guess just search for `git forcibly merge` online and do that.