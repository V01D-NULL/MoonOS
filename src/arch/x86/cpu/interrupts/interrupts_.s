;global isr0
;isr0:
;	cli                  ; Disable interrupts
;	push byte 0          ; Push a dummy error code (if ISR0 doesn't push it's own error code)
;	push byte 0          ; Push the interrupt number (0)
;	jmp isr_common_stub  ; Go to our common handler

; This macro creates a stub for an ISR which does NOT pass it's own
; error code (adds a dummy errcode byte).
%macro ISR_NOERRORCODE 1
	global isr%1
	isr%1:
		cli                  ; Disable interrupts
		push byte 0          ; Push a dummy error code (if ISRx doesn't push it's own error code)
		push byte %1         ; Push the interrupt number
		jmp isr_common_stub  ; Go to our common handler
%endmacro

; This macro creates a stub for an ISR which passes it's own error code.
%macro ISR_ERRORCODE 1
	global isr%1
	isr%1:
		cli                  ; Disable interrupts
		push byte %1         ; Push the interrupt number
		jmp isr_common_stub  ; Go to our common handler
%endmacro

; This macro creates a stub for an IRQ - the first parameter is
; the IRQ number, the second is the ISR number it is remapped to.
%macro IRQ 2
	global irq%1
	irq%1:
		cli                  ; Disable interrupts
		push byte 0          ; Push a dummy error code
		push byte %2         ; Push the interrupt number
		jmp irq_common_stub  ; Go to our common handler
%endmacro

; expand macro
ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE   8
ISR_NOERRORCODE 9
ISR_ERRORCODE   10
ISR_ERRORCODE   11
ISR_ERRORCODE   12
ISR_ERRORCODE   13
ISR_ERRORCODE   14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_NOERRORCODE 17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31

IRQ  0, 32
IRQ  1, 33
IRQ  2, 34
IRQ  3, 35
IRQ  4, 36
IRQ  5, 37
IRQ  6, 38
IRQ  7, 39
IRQ  8, 40
IRQ  9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

extern isr_handler           ; in isr.c
extern irq_handler           ; in isr.c

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:

	pusha                    ; Save edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov  ax, ds              ; Lower 16-bits of eax = ds.
	push eax                 ; Save the data segment descriptor

	mov  ax, 0x10            ; Load the kernel data segment descriptor
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax

	call isr_handler

	pop  eax                 ; Restore the original data segment descriptor
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax

	popa                     ; Restore edi,esi,ebp...

	add  esp, 8              ; Cleans up the pushed error code and pushed ISR number

	sti                      ; Enable interrupts

	iret                     ; Return from interrupt, and restore processor state
	                         ; Pops CS, SS, EFLAGS, ESP, EIP

; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
; irq_handler is subtly different from isr_handler, because before you return 
; must inform PIC have finished handling the interrupt
irq_common_stub:

	pusha                    ; Save edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov  ax, ds              ; Lower 16-bits of eax = ds.
	push eax                 ; Save the data segment descriptor

	mov  ax, 0x10            ; Load the kernel data segment descriptor
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax

	call irq_handler

	pop  eax                 ; Restore the original data segment descriptor
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax

	popa                     ; Restore edi,esi,ebp...

	add  esp, 8              ; Cleans up the pushed error code and pushed ISR number

	sti                      ; Enable interrupts

	iret                     ; Return from interrupt, and restore processor state
	                         ; Pops CS, SS, EFLAGS, ESP, EIP

