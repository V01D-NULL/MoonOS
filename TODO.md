### Todo list

IMPORTANT: Remember to update the README and resume if this works out.

- sched/scheduler.c and sched/task.c should not be performing context switches since that hardcodes architecture specific registers. Let arch/something/idk_which_folder_yet do it and invoke appropriate functions.

- Replace `struct stivale2_struct_tag_modules *mods` with something like `struct HandoverModules *`. If used with limine it just contains pointers to the beginning of each module and their sizes. If used with something that doesn't have a bootloader like the 3ds, read the modules from the SD car before passing control to `kern_main`. panic() if that goes wrong.

### Misc
When it comes to merging, I guess just search for `git forcibly merge` online and do that.