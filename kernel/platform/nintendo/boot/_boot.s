.section .text
.extern bootstrap_kernel, __stack_top__

.globl boot
boot:
	ldr sp, =__stack_top__
	mov fp, #0
	
	bl bootstrap_kernel

	1:
		wfi
		b 1b
