nasm -felf64 boot.s
gcc -m64 -ffreestanding -c main.c -o main.o
ld -n -o kernel.elf -T link.ld boot.o main.o
cp kernel.elf iso/boot/kernel.elf
cp grub.cfg iso/boot/grub/grub.cfg
grub-mkrescue -o experimental.iso iso
qemu-system-x86_64 -cdrom experimental.iso