/**
 * @file idt.h
 * @author Tim (V01D)
 * @brief Interrupt Descriptor Table
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include "../common.h"
#include "../amd64/cpu.h"

/**
 * @brief Defines an IDT entry
 * 
 */
struct idt_desc {
    uint16_t offset_low;  // offset bits 0..15
    uint16_t selector;    // a code segment selector in GDT or LDT
    uint8_t  ist;         // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attr;   // type and attributes
    uint16_t offset_mid;  // offset bits 16..31
    uint32_t offset_high; // offset bits 32..63
    uint32_t zero;        // reserved
}__attribute__((packed));

/**
 * @brief The values used to load the IDT into memory using 'lidt'
 * 
 */
struct idtr {
    uint16_t limit;
    uint64_t offset;
}__attribute__((packed));

/**
 * @brief Set an IDT entry
 * 
 * @param selector  Code segment
 * @param ist       Interrupt stack table offset
 * @param type_attr Types and attributes
 * @param offset    Low, mid, and high offset
 * @param idx       IDT entry index
 */
void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset, uint8_t idx);

/**
 * @brief Setup and load IDT
 * 
 */
void init_idt();

//CPU exceptions
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

//HW interrupts
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42(); 
extern void isr43(); 
extern void isr44(); 
extern void isr45(); 
extern void isr46(); 
extern void isr47();

#endif // IDT_H