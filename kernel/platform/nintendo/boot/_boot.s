.section .text
.extern bootstrap_kernel

.globl boot
boot:
	ldr sp, =0x1FFFE000
	b bootstrap_kernel

	1:
		b 1b
