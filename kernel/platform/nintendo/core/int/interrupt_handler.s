.extern a11_exception_main

.macro GEN_HANDLER label, id
.globl \label
\label:
	mov r0, #\id
	b asm_exception_stub
.endm

@ TODO: Due to the 3ds's firmware the regular IVT isn't present (and it doesn't start at address 0 either)
@ Once paging is enabled a new IVT will be created and loaded.
@ Once that is complete SVC will be available.
GEN_HANDLER fiq_handler, 0
GEN_HANDLER irq_handler, 1
GEN_HANDLER ud_handler, 2
GEN_HANDLER unknown_handler, 3
GEN_HANDLER prefetch_abort_handler, 4
GEN_HANDLER data_abort_handler, 5

.globl asm_exception_stub
asm_exception_stub:
	ldr sp, =0x1FFFE000 @ TODO: Is this stack placement okay?
	push {r0}
	bl a11_exception_main
