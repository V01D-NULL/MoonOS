#include "interrupts.h"
#include "idt.h"
#include <stddef.h>
#include <stdint.h>

isr_t isr_handler_array[256];

//Straight up copy and paste the text from the intel programming manual :D
volatile static const char* exception_messages[31] = {
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
   "Type: (NONE) Reserved"
};

void isr_handler(regs_t regs)
{
    /* CPU exceptions */
    if (regs.isr_number < 32)
    {
        if (regs.isr_number == 13) {
            debug("GPF(0): eflags.NT=%d\n", regs.rflags >> 14);
            debug("GPF(0): cpl@ss=%d\n", regs.ss & 3);
        }
        set_color(VGA_BLACK, VGA_LIGHT_RED);
        serial_set_color(BASH_RED);
        kprintf("[INTR] %s (err_code %d)\n", exception_messages[regs.isr_number], regs.error_code);
        debug("INT#%d - %s (err_code %d)\n", regs.isr_number, exception_messages[regs.isr_number], regs.error_code);
        serial_set_color(BASH_WHITE);
        debug("Register dump:\n"                      \
                "rax %x, rbx %x, rcx %x, rdx %x\n"    \
                "rbp %x, rsp %x, rdi %x, rsi %x\n"    \
                "rip %x, cs  %x, ss  %x, rflags %x\n" \
                "r8  %x, r9  %x, r10 %x, r11  %x\n"   \
                "r12 %x, r13 %x, r14 %x, r15  %x\n",
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
                                                                regs.r15
                                                                );
        
        set_color(VGA_BLACK, VGA_WHITE);
        
        asm("nop");
        asm("nop");
        asm("nop");
        asm("hlt");
    }
    
    if (isr_handler_array[regs.isr_number] != 0)
    {
        isr_t handler = isr_handler_array[regs.isr_number];
        handler(regs);
    }

    else {
        kprintf("Unhandled interrupt: 0x%d\n", regs.isr_number);
    }
    // if (regs->isr_number == 80)
    // {
    //     kprintf("this is custom interrupt 0x80\n");
    //     //Causes a GPF?
    // }
    /* Todo: Handle isr's 48-255 ie. user defined isr's (note: isr's 32-47 are marked as IRQ's and handled separately */
}

void install_isr(uint8_t base, isr_t handler)
{
    if (isr_handler_array[base] == 0)
        isr_handler_array[base] = handler;
    else
        kprintf("This interrupt has already been registered!\n");
}

void uninstall_isr(uint8_t base)
{
    isr_handler_array[base] = 0;
}