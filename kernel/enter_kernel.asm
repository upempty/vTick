;this needs to be linked with the kernel c file
[bits 32]

[extern kern_start]
call kern_start ;call the C function

jmp $


global isr_exit
isr_exit:           ; used for newly created tasks in order to skip having to build the entire return stack
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8      ; pop error code and interrupt number
    iret            ; pops (CS, EIP, EFLAGS) and also (SS, ESP) if privilege change occurs



extern current_task
extern tss

; void switch_context(Task* old, Task* new);
; pushes register state onto old tasks kernel stack (the one we're in right now) and
; pops the new state, making sure to update the kesps
global switch_context
switch_context:
    mov eax, [esp + 4] ; eax = old
    mov edx, [esp + 8] ; edx = new

    ; push registers that arent already saved by cdecl call etc.
    push ebp
    push ebx
    push esi
    push edi

    ; swap kernel stack pointer
    mov [eax + 4], esp ; old->kesp = esp
    mov esp, [edx + 4] ; esp = new->kesp

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret ; new tasks change the return value using TaskReturnContext.eip