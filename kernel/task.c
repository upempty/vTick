
#include "../include/task.h"
#include "../include/mm_paging.h"
#include "../include/mm_heap.h"
#include "../libc/mem.h"
#include "../include/tty.h"
#include "../include/gdt.h"



void switch_pgd(uint32_t pd)
{
    asm volatile ("mov %0, %%cr3" : : "r"(pd));
}


pid_t now_pid = 0;

tcb_t *running_task_head = NULL;
tcb_t *wait_task_head = NULL;
tcb_t *current = NULL;



int32_t kernel_task_create(int (*fn)(void ), void *arg)
{
    tcb_t *new_task = (tcb_t *)kmalloc(STACK_SIZE);

    bzero(new_task, sizeof(tcb_t));

    // pd are shared by kenrel.
    new_task->cr3   = (uint32_t)identityDirectory;
    new_task->state = TASK_RUNNABLE;
    new_task->pid   = now_pid++;
    set_task_name(new_task, "tcb-");
    new_task->kstack = (uint32_t)new_task + STACK_SIZE;


    uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)ktask_exit;
    *(--stack_top) = (uint32_t)fn;

    new_task->context.esp = (uint32_t)new_task + STACK_SIZE - 3 * sizeof(uint32_t);

    new_task->context.eflags = 0x200;


    new_task->next = running_task_head;
    new_task->prev = running_task_head->prev;
    running_task_head->prev->next = new_task;
    running_task_head->prev = new_task;

    return new_task->pid;
}

void ktask_exit()
{
    kprint("kernel task exit");
}

char * set_task_name(tcb_t *task, const char *name) {
    bzero(task->name, sizeof(task->name));
    return strcpy(task->name, name);
}

int32_t getpid()
{
    return current->pid;
}

void init_dispatcher()
{
    current = (tcb_t *)kern_stack;
    current->state = TASK_RUNNABLE;
    current->pid = now_pid++;
    set_task_name(current, "idletask");
    current->cr3 = (uint32_t)identityDirectory;
    current->kstack = (uint32_t)kern_stack + STACK_SIZE;

    current->next = current;
    current->prev = current;
    running_task_head = current;
}

void dispatch()
{
    if (current) {
         change_task_to(current->next);
    }
}

void change_task_to(tcb_t *next)
{
    if (current != next) {
        tcb_t *prev = current;
        current = next;

        set_kernel_stack(next->kstack);
        switch_pgd(next->cr3);
        switch_to(&(prev->context), &(current->context));
    }
}
