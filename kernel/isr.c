#include "../include/isr.h"
#include "../include/idt.h"
#include "../include/tty.h"
#include "../include/ports.h"
#include "../libc/strings.h"
#include "../include/timer.h"

isr_t interrupt_handlers[256];

void isr_install()
{
    set_idt_gate(0, (u32)isr0);
    set_idt_gate(1, (u32)isr1);
    set_idt_gate(2, (u32)isr2);
    set_idt_gate(3, (u32)isr3);
    set_idt_gate(4, (u32)isr4);
    set_idt_gate(5, (u32)isr5);
    set_idt_gate(6, (u32)isr6);
    set_idt_gate(7, (u32)isr7);
    set_idt_gate(8, (u32)isr8);
    set_idt_gate(9, (u32)isr9);
    set_idt_gate(10, (u32)isr10);
    set_idt_gate(11, (u32)isr11);
    set_idt_gate(12, (u32)isr12);
    set_idt_gate(13, (u32)isr13);
    set_idt_gate(14, (u32)isr14);
    set_idt_gate(15, (u32)isr15);
    set_idt_gate(16, (u32)isr16);
    set_idt_gate(17, (u32)isr17);
    set_idt_gate(18, (u32)isr18);
    set_idt_gate(19, (u32)isr19);
    set_idt_gate(20, (u32)isr20);
    set_idt_gate(21, (u32)isr21);
    set_idt_gate(22, (u32)isr22);
    set_idt_gate(23, (u32)isr23);
    set_idt_gate(24, (u32)isr24);
    set_idt_gate(25, (u32)isr25);
    set_idt_gate(26, (u32)isr26);
    set_idt_gate(27, (u32)isr27);
    set_idt_gate(28, (u32)isr28);
    set_idt_gate(29, (u32)isr29);
    set_idt_gate(30, (u32)isr30);
    set_idt_gate(31, (u32)isr31);

    //to remap the PIC to 0x20 and 0x28
    port_byte_out(PIC_master_cmd, 0x11); /* init command 1 - cascade, edge triggered */
    port_byte_out(PIC_slave_cmd, 0x11);
    
    //new offsets to handle using of same memory spaces by IRQs and IDTs
    port_byte_out(PIC_master_data, 0x20); /* init command 2 - redirect irqs 0-7 to vector 32 */
    port_byte_out(PIC_slave_data, 0x28); /* init command 2 - redirect irqs 8 - 15 to vector 40 */
    //give them their roles 4-master 2-slave
    port_byte_out(PIC_master_data, 0x04); /* init command 3 - slave on irq 2 */
    port_byte_out(PIC_slave_data, 0x02);
    //set to 8086/8088 mode (virtual real mode) - real mode apps in protected mode
    port_byte_out(PIC_master_data, 0x01); /* init command 4 - nonbuffered - 80x86 mode */
    port_byte_out(PIC_slave_data, 0x01); /* init command 4 - 80x86 mode */
    //masks
    port_byte_out(PIC_master_data, 0x0);
    port_byte_out(PIC_slave_data, 0x0);

    /*
      mask off all irqs except keyboard and timer  
      outb(0x21, 0xFC);		 irqs 0 - 8  
      outb(0xA1, 0xFF);		 irqs 8 - 15  
    */

    set_idt_gate(32, (u32)irq0);
    set_idt_gate(33, (u32)irq1);
    set_idt_gate(34, (u32)irq2);
    set_idt_gate(35, (u32)irq3);
    set_idt_gate(36, (u32)irq4);
    set_idt_gate(37, (u32)irq5);
    set_idt_gate(38, (u32)irq6);
    set_idt_gate(39, (u32)irq7);
    set_idt_gate(40, (u32)irq8);
    set_idt_gate(41, (u32)irq9);
    set_idt_gate(42, (u32)irq10);
    set_idt_gate(43, (u32)irq11);
    set_idt_gate(44, (u32)irq12);
    set_idt_gate(45, (u32)irq13);
    set_idt_gate(46, (u32)irq14);
    set_idt_gate(47, (u32)irq15);

    set_idt();
}

char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

void register_interrupt_handler(u8 n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_table r)
{
    kprint("received interrupt: ");
    char s[3];
    int_to_ascii(r.int_no, s);
    kprint(s);
    kprint("\n");
    kprint(exception_messages[r.int_no]);
    kprint("\n\n");
}

void irq_handler(registers_table r)
{
    //send EOI to both PICs
    if (r.int_no >= 40)
    {
        port_byte_out(PIC_slave_cmd, PIC_EOI);
    }
    port_byte_out(PIC_master_cmd, PIC_EOI);

    if (interrupt_handlers[r.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}

void irq_install()
{
    asm volatile("sti");
    // testing 18.21 Hz
    init_timer(65535); //65535 1193180
    //init_timer(50);
}
