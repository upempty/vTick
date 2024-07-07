[org 0x7c00]

boot_start: 
    mov [BOOT_DRIVE], dl ;boot drive is set in dl by BIOS

    ;setting up stack
    mov bp,0x7c00 
    mov sp, bp

    mov bx, GREETING_REAL_MODE
    call print_string
    call print_newline

    call load_vbr; call second stage
    call switch_to_pm

    jmp $ ;never executed to here

;---------------------------------------------------------------------------------
[bits 16]
print_string:
    ;parameter: bx
    pusha

    start:
        mov al, [bx]
        cmp al, 0
        je done

        mov ah, 0x0e ;set to TTY mode
        int 0x10

        add bx, 1
        jmp start
    
    done:
        popa
        ret


print_newline:
    pusha

    mov ah, 0x0e ;set to TTY mode
    mov al, 0x0a ;set to newline character
    int 0x10
    mov al, 0x0d ;set to carriage return
    int 0x10

    popa
    ret


print_hex_data:
    ;parameter: dx
    pusha

    mov cx, 0 ;the index 

    hex_loop:
        cmp cx,4
        je end

        mov ax,dx
        and ax,0x000f
        add al,0x30
        cmp al,0x39
        jle step2
        add al,7

        step2:
            mov bx,HEX_OUT + 5
            sub bx,cx 
            mov [bx], al 
            ror dx,4

            add cx,1
            jmp hex_loop

    end:
        mov bx,HEX_OUT
        call print_string

        popa
        ret

    HEX_OUT:
        db '0x0000',0


gdt_start:
    ;needs 8 null bytes to start
    dd 0x0 ;4 bytes
    dd 0x0 ;4 bytes


;descriptor for code segement
gdt_code:
    ;base has to be 0x0
    ;limit is 0xfffff
    ;1st flags - present:1, privilege:00, descriptor type: 1 > 1001
    ;type flags - code:1, conforming:0, readable:1, accessed:0 > 1010
    ;2nd flags: granularity:1,32-bit default:1,64-bit seg:0,AVL,0 > 1100

    dw 0xffff    ;segment length mltiplier-ish - limit - (bits 0-15)
    dw 0x0       ;Base address (bits 0-15)
    db 0x0       ;Base address (bits 16-23)
    db 10011010b ;1st flags, type flags
    db 11001111b ;2nd flags, limit - (bits 16-19)
    db 0x0       ;Base address (bits 24-31) 

gdt_data:
    ;type flags - code:0, expand down:0, writable: 1, accessed:0 >0010
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b ;1st flags, type flags
    db 11001111b
    db 0x0

gdt_end:
    ;only to calculate the size of the GDT

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start ;address of the descriptor (32-bit)

[bits 32]
pm_print_string:
    pusha
    mov edx,VIDEO_MEMORY

    pm_print_string_loop:
        mov al, [ebx] ;char at ebx
        mov ah, WHITE_ON_BLACK ;color attributes in ah

        cmp al, 0
        je pm_print_done

        mov [edx], ax

        add ebx, 1
        add edx, 2

        jmp pm_print_string_loop

    pm_print_done:
        popa
        ret

[bits 16] ;16-bit instructions to switch to 32-bit pm
switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1 ;set pm(32 bit) mode in cr0
    mov cr0, eax
    jmp CODE_SEGMENT:prepare_stage2


[bits 32]
prepare_stage2:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x1000000; 64k ok, here to use below 16M. and above 16M for kernel code.
    ;                   1M not ok why? because e820 mem layout reserves for <1M.
    mov esp, ebp

    mov ebx, MSG_PM_ENTRY
    call pm_print_string
    call SECOND_STAGE_OFFSET ; Go to stage2.
    jmp $



;bx will have to be set to the position of the vbr offset
disk_load:
    pusha ;
    push dx ;input parameter - save to stack for later use

    mov ah,0x02 ; read instruction
    mov al,dh ; number of sectors to be read
    mov cl,0x02 ; the 2th sector to be read from, and note that 0x01 is the first sector/boot sector
    mov ch, 0x00 ; cylinder number
    mov dh, 0x00 ; head number

    int 0x13
    jc disk_error

    pop dx
    cmp al,dh
    jne sectors_error
    popa
    ret

    sectors_error:
        mov bx,SECTORS_ERROR_STRING
        call print_string
        call print_newline
        mov dh,ah ;ah contains the error code
        call print_hex_data
        jmp disk_error_loop

    disk_error:
        mov bx, DISK_ERROR_STRING
        call print_string


disk_error_loop:
    jmp $

[bits 16]
load_vbr: ;load 2th stage from disk
    mov bx, MSG_LOAD_VBR
    call print_string
    call print_newline

    mov bx, SECOND_STAGE_OFFSET
    mov dh, 7 ;number of sectors to read
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret


;data
BOOT_DRIVE db 0
GREETING_REAL_MODE db 'Starting boot in 16-bit real mode.',0
MSG_PM_ENTRY db 'Enter in 32-bit protected mode.',0
MSG_LOAD_VBR db 'Loading VBR into memory from disk.',0

CODE_SEGMENT equ gdt_code - gdt_start
DATA_SEGMENT equ gdt_data - gdt_start

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f ;color byte

SECOND_STAGE_OFFSET equ 0x7e00

DISK_ERROR_STRING:
    db "Disk read error",0
SECTORS_ERROR_STRING:
    db "Incorrent number of sectors were read",0

;padding with 510 - size of code
times 510 - ($-$$) db 0
dw 0xaa55
