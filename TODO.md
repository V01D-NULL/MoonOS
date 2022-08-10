### Todo list

- Make the limine-terminal-port generic code. It should use an extern function to allocate memory and render pixels/characters (probably just characters). The character rendering would be done in platform/foobar/framebuffer/lfb.{c,h}

- Add `arch_reboot_after_confirmation` which waits for a keypress or something and reboots. Could be used in `panic()`.

- Replace mmio code (example: apic, hpet) with the `mmio_*()` functions

- Modify the gensym script to generate a symbol table for arm and x86 respectively (mainly eye-balling the hardcoded x86 gcc here. Probably just take the compiler and it's arguments in sys.argv or something)

- Replace `struct stivale2_struct_tag_modules *mods` with something like `struct HandoverModules *`. If used with limine it just contains pointers to the beginning of each module and their sizes. If used with something that doesn't have a bootloader 
like the 3ds, read the modules from the SD car before passing control to `kern_main`. panic() if that goes wrong.

- Once the 3ds has an SD Card driver, write a kernel panic to a text file or something.

- Once the armv6 port is stable, search for `__x86_64__` in the code since it was a quick hack to get the PoC up and running. Don't wanna keep that.
