#include "../include/tty.h"
#include "../include/mm_heap.h"
#include "../include/mm_paging.h"
#include "../include/gdt.h"
#include "../include/isr.h"
#include "../include/task.h"


void kern_start()
{
    /* if clear, then will overwrite 'Enter PM mode log', 
    *  should be expected behaviour.*/
    //kclear_tty(); 
    //to refactor this stack init.
    kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kern_stack_top));
            
    kprint_at("Welcome Fred, you are in kernel now, step back!\n", 10, 0);

    //to debug
    kheap_init();
       kprint_at("Welcome Fred, you are in kernel now, Heap!\n", 11, 0);
    mm_paging_init();
       //kprint_at("Welcome Fred, you are in kernel now, Paging!\n", 12, 0);
    gdt_setup();
       kprint_at("Welcome Fred, you are in kernel now, GDT!\n", 13, 0);
    // testing
    isr_install();
    irq_install();
       kprint_at("Welcome Fred, you are in kernel now, isr/irq!\n", 14, 0);
    init_dispatcher();
    kernel_task_create(task_one, NULL);
    kernel_task_create(task_two, NULL); 
    
    while (true) {
        kprint("0-task_idle\n");
        asm volatile("hlt");
    }

}
