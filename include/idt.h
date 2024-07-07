#ifndef IDT_H
#define IDT_H

#include "dtypes.h"

#define KERNEL_CS 0x08

typedef struct{
    u16 low_offset; //low 16 bits of handler function
    u16 sel;        //segment  selector
    u8 always0;
    u8 flags; /*7: Interrupt present,
                6-5 Privilege level 0=kernel,3=user,
                4 0 for interrupt gate
                3-0 1110 for dec 14 - 32 bit interrupt gate*/
    u16 high_offset; //high 16 bites of handlers
} __attribute__((packed)) idt_gate_type;


typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_register_type;

#define IDT_ENTRIES 256
idt_gate_type idt[IDT_ENTRIES];
idt_register_type idt_reg;

void set_idt_gate(int n, u32 handler);
void set_idt();

#endif