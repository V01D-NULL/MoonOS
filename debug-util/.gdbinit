file kernel/kernel.elf

target remote :1234

set print pretty on

set disassembly-flavor intel

b *kinit

continue
