global _start

extern main

_start:
	call main
	jmp $
