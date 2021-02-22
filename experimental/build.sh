nasm -felf32 boot.s
ld -n -o kernel.bin -T link.ld boot.o
