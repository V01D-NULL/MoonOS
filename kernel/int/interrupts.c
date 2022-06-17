#include "interrupts.h"
#include "idt.h"
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <mm/vmm.h>
#include <amd64/bytes.h>
#include <stddef.h>
#include <stdint.h>
#include <printk.h>
#include <panic.h>
#include <proc/uspace/syscalls.h>

isr_t isr_handler_array[256] = {0};
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

void isr_handler(struct iframe regs)
{
    /* CPU exceptions */
    if (regs.isr_number < 32)
    {
		if (regs.cs == 0x43)
		{
            asm volatile("swapgs" ::: "memory");
			// printk("INT", BASH_GREEN "Userprocess triggered an exception\n" BASH_GRAY);
            debug(true, BASH_GREEN "Userprocess triggered an exception\n" BASH_GRAY);
		}

        override_quiet_boot();
        serial_set_color(BASH_RED);
        debug(true, "ERROR: %s (err_code %ld)\n", exception_messages[regs.isr_number], regs.error_code);
        // printk("INT", "%s (err_code %ld)\n", exception_messages[regs.isr_number], regs.error_code);

        if (regs.isr_number == 14)
        {
            uint64_t cr2 = cr_read(CR2);
            // printk("INT ~ #PF", "Faulting address: 0x%lx\n", cr2);
            debug(true, "INT ~ #PF Faulting address: 0x%lx\n", cr2);
            // for(;;);
            pagefault_handler(cr2, regs.error_code);
            goto exit_handler;
        }
         if (regs.isr_number == 6)
        {
            printk("INT", "Faulting instruction is at: 0x%llx\n", regs.rip);
            uint16_t instruction = *(uint64_t*)regs.rip;
            instruction = byte_order_reverse_short16(instruction);
            printk("INT", "Opcode: %x\n", (instruction));
            
            if (instruction == 0x0F05 /* syscall */)
                printk("INT", "TODO: Handle SYSCALL #UD and use int $0x80 instead!");
        }

        debug(true, "INT#%d - %s (err_code %ld)\n", regs.isr_number, exception_messages[regs.isr_number], regs.error_code);
        serial_set_color(BASH_WHITE);
        debug(false, "Register dump:\n"
                     "rax %p | rbx %p | rcx %p | rdx    %p\n"
                     "rbp %p | rsp %p | rdi %p | rsi    %p\n"
                     "rip %p | cs  %p | ss  %p | rflags %p\n"
                     "r8  %p | r9  %p | r10 %p | r11    %p\n"
                     "r12 %p | r13 %p | r14 %p | r15    %p\n",
              regs.gpr.rax,
              regs.gpr.rbx,
              regs.gpr.rcx,
              regs.gpr.rdx,
              regs.gpr.rbp,
              regs.rsp,
              regs.gpr.rdi,
              regs.gpr.rsi,
              regs.rip,
              regs.cs,
              regs.ss,
              regs.rflags,
              regs.gpr.r8,
              regs.gpr.r9,
              regs.gpr.r10,
              regs.gpr.r11,
              regs.gpr.r12,
              regs.gpr.r13,
              regs.gpr.r14,
              regs.gpr.r15);

        if (!canReturn)
        {
            for (;;)
                asm("hlt");
        }
    }

    TRY_EXEC_HANDLER(regs.isr_number);

    //Signal EOI
    if (regs.isr_number > 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);

exit_handler:
    if (regs.cs == 0x43)
        asm volatile("swapgs" ::: "memory");
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

bool is_isr_registered(uint8_t isr)
{
    return isr_handler_array[isr] != 0;
}