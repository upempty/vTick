#include "../include/timer.h"
#include "../include/tty.h"
#include "../include/isr.h"
#include "../include/ports.h"
#include "../include/task.h"


#include "../libc/strings.h"
#include "../libc/function.h"


u32 tick = 0;

static void timer_callback(registers_table regs){
    tick++;

    UNUSED(regs);

    if (tick % 10000 == 0) {
         dispatch(); 
    }
    
}

void init_timer(u32 frequency){
    register_interrupt_handler(IRQ0, timer_callback); //IRQ0 is for the Programmable Interrupt Timer

    u32 divisor = 1193180 / frequency; //the divided the default frequency to give the required frequency
    u8 low = (u8)(divisor & 0xFF);
    u8 high = (u8)((divisor >> 8) & 0xFF);

    port_byte_out(0x43, 0x36); //command
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
