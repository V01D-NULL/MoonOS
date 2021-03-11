#include "idt.h"
#include <stddef.h>
#include <stdint.h>

static struct idt_desc idt[256];

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

void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset /*offset2 + offset3*/, uint8_t idx)
{
    idt[idx].offset_low  = offset & 0xFFFFFFFF;
    idt[idx].selector    = selector;
    idt[idx].ist         = ist;
    idt[idx].type_attr   = type_attr;
    idt[idx].offset_mid  = (offset >> 16) & 0xFFFFFFFF;
    idt[idx].offset_high = (offset >> 32) & 0xFFFFFFFF;
    idt[idx].zero        = 0;
}

void isr_handler(struct regs *regs)
{
    /* CPU exceptions */
    if (regs->isr_number < 32)
    {
        set_color(VGA_BLACK, VGA_LIGHT_RED);
        serial_set_color(BASH_RED);
        kprintf("[INTR] %s (err_code %d)\n", exception_messages[regs->isr_number], regs->error_code);
        debug("INT#%d - %s (err_code %d)\n", regs->isr_number, exception_messages[regs->isr_number], regs->error_code);
        serial_set_color(BASH_WHITE);
        debug("Register dump:\n"                      \
                "rax %x, rbx %x, rcx %x, rdx %x\n"    \
                "rbp %x, rsp %x, rdi %x, rsi %x\n"    \
                "rip %x, cs  %x, ss  %x, rflags %x\n" \
                "r8  %x, r9  %x, r10 %x, r11  %x\n"   \
                "r12 %x, r13 %x, r14 %x, r15  %x\n",
                                                                regs->rax,
                                                                regs->rbx,
                                                                regs->rcx,
                                                                regs->rdx,
                                                                regs->rbp,
                                                                regs->rsp,
                                                                regs->rdi,
                                                                regs->rsi,
                                                                regs->rip,
                                                                regs->cs,
                                                                regs->ss,
                                                                regs->rflags,
                                                                regs->r8,
                                                                regs->r9,
                                                                regs->r10,
                                                                regs->r11,
                                                                regs->r12,
                                                                regs->r13,
                                                                regs->r14,
                                                                regs->r15
                                                                );
        
        set_color(VGA_BLACK, VGA_WHITE);

        asm("hlt");
    }
    /* Todo: Handle isr's 48-255 ie. user defined isr's (note: isr's 32-47 are marked as IRQ's and handled separately */
}

extern void load_idt(uint64_t idtr);

void init_idt()
{
    monitor_write("Initialising interrupts\n", true, false);
    // idt_set_entry(uint16_t selector /*kernel CS (0x08)*/, uint8_t ist /*0*/, uint8_t type_attr /*0x8E*/, uint64_t offset /*asm func*/, uint8_t idx /*isr index*/)
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr0, 0);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr1, 1);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr2, 2);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr3, 3);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr4, 4);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr5, 5);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr6, 6);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr7, 7);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err8, 8);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr9, 9);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err10, 10);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err11, 11);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err12, 12);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err13, 13);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err14, 14);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr15, 15);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr16, 16);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr17, 17);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr18, 18);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr19, 19);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr20, 20);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr21, 21);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr22, 22);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr23, 23);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr24, 24);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr25, 25);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr26, 26);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr27, 27);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr28, 28);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr29, 29);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr30, 30);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr31, 31);
    
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq0, 32);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq1, 33);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq2, 34);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq3, 35);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq4, 36);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq5, 37);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq6, 38);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq7, 39);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq8, 40);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq9, 41);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq10, 42);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq11, 43);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq12, 44);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq13, 45);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq14, 46);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)irq15, 47);

    /*
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr48, 48);
    ...
    */

    // Load IDT
    struct idtr _idtr = { 
        .limit  = 256 * sizeof(struct idt_desc) - 1, 
        .offset = (uint64_t)idt
    };
    
    load_idt((uint64_t)&_idtr);
    
    monitor_write("Initialised interrupts\n", false, true);
}