[bits 32]


global cpu_vendor_string

cpu_vendor_string:
	;
    ; eax = The value that will be returned to C
    ; 

    xor eax, eax       ; EAX = 0 = Get vendor string
	
    cpuid              ; Dew it
    mov [eax], ebx     ; load first 4 chars from ebx
    mov [eax+4], edx   ; load next 4 characters from edx
    mov [eax+8], ecx   ; load next 4 characters from ecx

	ret
