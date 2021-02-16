;; Kernel start location. Also defines multiboot header.

; http://www.osdever.net/bkerndev/index.php
; https://github.com/berkus/jamesm-tutorials/blob/master/src/boot.s

MBOOT_PAGE_ALIGN    equ 1 << 0      ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1 << 1      ; Provide your kernel with memory info
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_HEADER_MAGIC  equ 0x1BADB002  ; Multiboot Magic value. Identifies kernel as multiboot-compatible
MBOOT_CHECKSUM      equ - ( MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS )

bits 32                             ; All instructions should be 32-bit.

; section .text

global mboot                        ; Make 'mboot' accessible from C.
extern code                         ; Start of the '.text' section.
extern bss                          ; Start of the .bss section.
extern end                          ; End of the last loadable section.

mboot:

	dd  MBOOT_HEADER_MAGIC          ; GRUB will search for this value on each
	                                ; 4-byte boundary in your kernel file
	dd  MBOOT_HEADER_FLAGS          ; How GRUB should load your file / settings
	dd  MBOOT_CHECKSUM              ; To ensure that the above values are correct

	dd  mboot                       ; Location of this descriptor
	dd  code                        ; Start of kernel '.text' (code) section.
	dd  bss                         ; End of kernel '.data' section.
	dd  end                         ; End of kernel.
	dd  start                       ; Kernel entry point (initial EIP).

global start                        ; Kernel entry point.
extern kmain                         ; This is the entry point of our C code

start:

	cli                             ; Disable interrupts.

	; mov esp, stack                  ; Set up our own stack.
	; mov ebp, 0                      ; Initialise the base pointer to zero so we can terminate stack traces here

	; Execute the kernel
	push  ebx                       ; Load multiboot header location
	call  kmain                      ; call our main() function.

	; Handle return
	jmp   $                         ; Enter an infinite loop, to stop the processor
	                                ; executing whatever rubbish is in the memory after our kernel!


; Here is the definition of our BSS section. Right now, we'll use
; it just to store the stack. Remember that a stack actually grows
; downwards, so we declare the size of the data before declaring
; the identifier '_sys_stack'
; SECTION .bss

; 	resb 8192                       ; This reserves 8KBytes of memory here

; stack:
