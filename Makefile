C_SOURCES = $(wildcard kernel/*.c libc/*.c)
HEADERS = $(wildcard include/*.h libc/*.c)
OBJ = ${C_SOURCES:.c=.o kernel/interrupts.o kernel/gdtflush.o kernel/switchit.o}

CFLAGS = -g -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nodefaultlibs -Wall -Wextra -Werror -fcommon

default: disk.img

disk.img: boot_mbr.bin boot_vbr.bin kernel.bin
	@echo " build  boot/boot.bin"
	dd of=$@ if=/dev/zero bs=512 count=1024 2>/dev/null
	dd of=$@ if=boot_mbr.bin conv=notrunc 2>/dev/null
	dd of=$@ if=boot_vbr.bin conv=notrunc bs=512 seek=1 2>/dev/null
	dd of=$@ if=kernel.bin conv=notrunc bs=512 seek=8 2>/dev/null
	@echo "[INFO]: Build completed successfully"

# special for kernel/enter_kernel.o position, must be precedence of other OBJs.
kernel.bin: kernel/enter_kernel.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000000 --oformat binary $^

boot_vbr.bin: boot/enter_vbr.o boot/boot_vbr.o
	ld -m elf_i386 -o $@ -Ttext 0x7e00 --oformat binary $^

# as [org 0x7c00] used, need this way to build bin for boot mbr. failed when asm->o as below.
boot_mbr.bin: boot/boot_mbr.asm
	nasm -f bin $^ -o $@

%.o: %.asm
	nasm $< -f elf -g -F stabs -o $@
#nasm $< -f elf -o $@
#nasm $< -f elf -g -F stabs -o $@

# ${CFLAGS} must be there
%.o: %.c
	gcc ${CFLAGS} -ffreestanding -m32 -fno-pic -c $< -o $@

.PHONY: clean
clean:
	rm -f *.bin *.o *.elf *.img
	rm -rf kernel/*.o boot/*.bin libc/*.o boot/*.o
