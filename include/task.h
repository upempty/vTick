#ifndef TASK_H
#define TASK_H

#include "dtypes.h"

#define TASK_NAME_LEN               16
#define MAX_TASK                 4096
#define MAX_PID                     (MAX_TASK * 2)

typedef enum task_state {
    TASK_UNINIT = 0,
    TASK_SLEEPING = 1,
    TASK_RUNNABLE = 2,
    TASK_ZOMBIE = 3,
} task_state_t;

typedef struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
} context_t;

typedef struct task_struct {
    volatile task_state_t state; 
    volatile bool need_resched;   
    context_t context;           
    uint32_t kstack;           
    uint32_t cr3;               
    pid_t pid;                    
    char name[TASK_NAME_LEN + 1];
    uint32_t flags;               
    struct task_struct *parent;   

    struct task_struct *prev;    
    struct task_struct *next;
} tcb_t;

extern pid_t now_pid;
char kern_stack[STACK_SIZE];

extern tcb_t *running_task_head;
extern tcb_t *wait_task_head;
extern tcb_t *current;

int32_t kernel_task_create(int (*fn)(void), void *arg);
void ktask_exit();
char *set_task_name(tcb_t *task, const char *name);
int32_t getpid();

void init_dispatcher();
void dispatch();
void change_task_to(tcb_t *next);
void switch_to(context_t *prev, context_t *next);

#endif
