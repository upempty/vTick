[bits 32]

[extern boot_main] ;the function in the vbr
call boot_main ;call the C function

jmp $