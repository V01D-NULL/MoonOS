### Todo list

IMPORTANT: Remember to update the README and resume if this works out.

- Different architectures need different makefiles (maybe have a Makefile.arch.platform.conf These files then execute specific build instructions for specific architectures & platforms)

- Replace `struct stivale2_struct_tag_modules *mods` with something like `struct HandoverModules *`. If used with limine it just contains pointers to the beginning of each module and their sizes. If used with something that doesn't have a bootloader like the 3ds, read the modules from the SD car before passing control to `kern_main`. panic() if that goes wrong.

### Misc
When it comes to merging, I guess just search for `git forcibly merge` online and do that.