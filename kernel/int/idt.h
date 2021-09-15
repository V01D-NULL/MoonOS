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
#include <util/common.h>
#include <amd64/cpu.h>
#include <amd64/validity.h>

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
} __pack_bytes;

/**
 * @brief The values used to load the IDT into memory using 'lidt'
 * 
 */
struct idtr {
    uint16_t limit;
    uint64_t offset;
} __pack_bytes;

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
__export void isr0();
__export void isr1();
__export void isr2();
__export void isr3();
__export void isr4();
__export void isr5();
__export void isr6();
__export void isr7();
__export void isr_err8();
__export void isr9();
__export void isr_err10();
__export void isr_err11();
__export void isr_err12();
__export void isr_err13();
__export void isr_err14();
__export void isr15();
__export void isr16();
__export void isr17();
__export void isr18();
__export void isr19();
__export void isr20();
__export void isr21();
__export void isr22();
__export void isr23();
__export void isr24();
__export void isr25();
__export void isr26();
__export void isr27();
__export void isr28();
__export void isr29();
__export void isr30();
__export void isr31();

//HW interrupts
__export void isr32();
__export void isr33();
__export void isr34();
__export void isr35();
__export void isr36();
__export void isr37();
__export void isr38();
__export void isr39();
__export void isr40();
__export void isr41();
__export void isr42(); 
__export void isr43(); 
__export void isr44(); 
__export void isr45(); 
__export void isr46(); 
__export void isr47();

#endif // IDT_H