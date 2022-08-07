.section .text
.extern main

.globl _start
_start:
	ldr sp, =0x1FFFE000
	1:
	b 1b
	@ b main
