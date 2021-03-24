#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include "../common.h"
#include "../drivers/vga/vga.h"

typedef struct regs {
    int64_t r15;
    int64_t r14;
    int64_t r13;
    int64_t r12;
    int64_t r11;
    int64_t r10;
    int64_t r9;
    int64_t r8;
    int64_t rsi;
    int64_t rdi;
    int64_t rbp;
    int64_t rdx;
    int64_t rcx;
    int64_t rbx;
    int64_t rax;
    int64_t isr_number;
    int64_t error_code;
    int64_t rip;
    int64_t cs; 
    int64_t rflags; 
    int64_t rsp;
    int64_t ss;
} regs_t;

struct idt_desc {
    uint16_t offset_low;  // offset bits 0..15
    uint16_t selector;    // a code segment selector in GDT or LDT
    uint8_t  ist;         // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attr;   // type and attributes
    uint16_t offset_mid;  // offset bits 16..31
    uint32_t offset_high; // offset bits 32..63
    uint32_t zero;        // reserved
}__attribute__((packed));

struct idtr {
    uint16_t limit;
    uint64_t offset;
}__attribute__((packed));

void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset, uint8_t idx);
void init_idt();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr_err8();
extern void isr9();
extern void isr_err10();
extern void isr_err11();
extern void isr_err12();
extern void isr_err13();
extern void isr_err14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void isr80();

#endif // IDT_H