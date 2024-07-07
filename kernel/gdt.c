#include "../include/mm_heap.h"
#include "../include/dtypes.h"
#include "../include/gdt.h"
#include "../libc/mem.h"

#define ASM	__asm__ __volatile__

typedef struct 
{
    unsigned int limit_l: 16;
    unsigned int base_l:  24;
    unsigned int access:   8;
    unsigned int limit_h:  4;
    unsigned int flags:    4;
    unsigned int base_h:   8;
} __attribute__ ((packed)) gdt_entry_t;

typedef struct
{
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed)) gdt_descriptor_t;

static  gdt_entry_t* gdt;
gdt_descriptor_t gdtr;
tss_entry_t tss_entry;


static void set_gdt_entry(int num, int base, int limit, unsigned char access, unsigned char flag) {
    gdt[num].base_l = base & 0xFFFFF;
    gdt[num].base_h = (base >> 24) & 0xFF;
    gdt[num].limit_l = (limit & 0xFFFF);
    gdt[num].limit_h = (limit >> 16) & 0xF;
    gdt[num].access = access;
    gdt[num].flags =  flag;
	/* 
    flag 4 bits uses 0xc(0x1100) 
    0b100 32-bit segment
    0b1000 scale the value of [limit] in the descriptor by 4kb, if not set, [limit] is scaled by 1
	*/
}

void gdt_reload(void)
{
	ASM( "lgdt (%0)" :: "r" ( &gdtr) );
	ASM( "movw %0, %%ax" :: "i" (KERNEL_DATA_SEL) );
	ASM( "movw %ax, %ds" );
	ASM( "movw %ax, %ss" );
	ASM( "movw %ax, %es" );
	ASM( "movw %ax, %fs" );
	ASM( "movw %ax, %gs" );
	ASM( "ljmp %0, $1f" :: "i" (KERNEL_CODE_SEL)  );	
	ASM( "1:" );
}


void gdt_setup(void) {
    gdt = kzalloc(4096);
    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0x000FFFFF, 0x9A, 0xC); //KERNEL_CODE_SEL ring 0, kernel mode
    set_gdt_entry(2, 0, 0x000FFFFF, 0x92, 0xC); //KERNEL_DATA_SEL
    set_gdt_entry(3, 0, 0x000FFFFF, 0xFA, 0xC); //USER_CODE_SEL ring 3, user mode: FC->FA
    set_gdt_entry(4, 0, 0x000FFFFF, 0xF2, 0xC); //USER_DATA_SEL	

    memset((uint8_t*) &tss_entry, 0, sizeof(tss_entry));
    tss_entry.ss0 = KERNEL_DATA_SEL;
    tss_entry.esp0 = 0;

    set_gdt_entry(5, (uint32_t) &tss_entry, sizeof(tss_entry), 0xe9, 0x0); // KERNEL_TSS_SEL 0x28

    gdtr.limit = 6 * sizeof(gdt_entry_t) - 1;
    gdtr.base = (int) gdt;

    gdt_flush((uint32_t)&gdtr);
    tss_flush();

}

void set_kernel_stack(uint32_t stack)
{
    tss_entry.esp0 = stack;
}




