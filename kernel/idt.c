#include "../include/idt.h"

idt_gate_type idt[IDT_ENTRIES];
idt_register_type idt_reg;

void set_idt_gate(int i, u32 handler)
{
    idt[i].low_offset = (u16)handler & 0x0000FFFF;
    idt[i].sel = KERNEL_CS;
    idt[i].always0 = 0;
    idt[i].flags = 0x8E;
    idt[i].high_offset = (u16)(handler >> 16) & 0x0000FFFF;
}

void set_idt()
{
    idt_reg.base = (u32)&idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_type) - 1;
    __asm__ __volatile__("lidt (%0)"
                         :
                         : "r"(&idt_reg));
}
