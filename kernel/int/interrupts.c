#include "interrupts.h"
#include "idt.h"
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <mm/vmm.h>
#include <amd64/bytes.h>
#include <util/ptr.h>
#include <stddef.h>
#include <stdint.h>
#include <printk.h>
#include <panic.h>

isr_t isr_handler_array[255] = {0};
bool canReturn = false;

static const char *exception_messages[] = {
    "Type: (#DE) Division Exception",
    "Type: (#DB) Debug Exception",
    "Type: (NONE) NMI interrupt (Non Maskable Interrupt)",
    "Type: (#BP) Breakpoint Exception",
    "Type: (#OF) Into detected Overflow Exception",
    "Type: (#BR) BOUND Range Exceeded",
    "Type: (#UD) Invalid Opcode (Undefined Opcode)",
    "Type: (#NM) Device Not Available (No Math Coprocessor)",
    "Type: (#DF) Double Fault",
    "Type: (NONE) Coprocessor Segment Overrun (reserved)",
    "Type: (#TS) Invalid TSS",
    "Type: (#NP) Segment Not Present",
    "Type: (#SS) Stack-Segment Fault",
    "Type: (#GP) General Protection Fault",
    "Type: (#PF) Page Fault",
    "Type: (NONE) Unknown interrupt",
    "Type: (#CF) Coprocessor fault",
    "Type: (#AC) Alignment check",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved"};

void isr_handler(regs_t regs)
{
    /* CPU exceptions */
    if (regs.isr_number < 32)
    {
		if (regs.cs == 0x43)
		{
			__asm__ volatile(
				gnu_asm_flavor_intel
				"mov ax, 0x30\n"
				"mov ds, ax\n"
				"mov fs, ax\n"
				"mov gs, ax\n"
				"mov ss, ax\n"
				"mov es, ax\n"
				gnu_asm_flavor_at_t
			);
			vmm_switch_to_kernel_pagemap();
			panic(":p");
			// printk("INT", "Todo: Terminate userprocess!");
			// goto end;
		}

        override_quiet_boot();
        serial_set_color(BASH_RED);
        printk("INT", "%s (err_code %ld)\n", exception_messages[regs.isr_number], regs.error_code);

        if (regs.isr_number == 14)
        {
            printk("INT ~ #PF", "Faulting address: 0x%lx\n", cr_read(CR2));
            
            uint64_t cr2 = cr_read(CR2);
            vmm_pagefault_handler(cr2, regs.error_code);
            return;
        }
        else if (regs.isr_number == 6)
        {
            printk("INT", "Faulting instruction is at: 0x%llx\n", regs.rip);
            printk("INT", "Opcode: %x\n", byte_order_reverse_char8(regs.rip & 0xff));
        }

        debug(true, "INT#%d - %s (err_code %ld)\n", regs.isr_number, exception_messages[regs.isr_number], regs.error_code);
        serial_set_color(BASH_WHITE);
        debug(false, "Register dump:\n"
                     "rax %p | rbx %p | rcx %p | rdx    %p\n"
                     "rbp %p | rsp %p | rdi %p | rsi    %p\n"
                     "rip %p | cs  %p | ss  %p | rflags %p\n"
                     "r8  %p | r9  %p | r10 %p | r11    %p\n"
                     "r12 %p | r13 %p | r14 %p | r15    %p\n",
              regs.rax,
              regs.rbx,
              regs.rcx,
              regs.rdx,
              regs.rbp,
              regs.rsp,
              regs.rdi,
              regs.rsi,
              regs.rip,
              regs.cs,
              regs.ss,
              regs.rflags,
              regs.r8,
              regs.r9,
              regs.r10,
              regs.r11,
              regs.r12,
              regs.r13,
              regs.r14,
              regs.r15);

        if (!canReturn)
        {
            for (;;)
                __asm__("hlt");
        }
    }

    TRY_EXEC_HANDLER(regs.isr_number);

    //Signal EOI
    if (regs.isr_number > 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void install_isr(uint8_t base, isr_t handler)
{
    if (isr_handler_array[base] == 0)
    {
        isr_handler_array[base] = handler;
    }
    else
        printk("INT", "The interrupt ( %d ) has already been registered!\n", base);
}
