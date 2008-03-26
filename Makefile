

patch_boot: patch_boot.c
	gcc -o patch_boot patch_boot.c

count_blocks: count_blocks.c
	gcc -o count_blocks count_blocks.c

bootblock: bootblock.s patch_boot bootloader count_blocks
	nasm -f bin bootblock.s
	./patch_boot bootblock `./count_blocks bootloader 512`

bootloader: bootloader.s patch_boot kern_img count_blocks
	nasm -f bin bootloader.s
	./patch_boot bootloader `./count_blocks kern_img 512`

kern_img: i386.s kernel.o i386lib.o
	nasm -f elf -o i386.o i386.s
	ld -N -e START -Ttext 0x10000 -o kern_img.out i386.o kernel.o i386lib.o
	objdump	-S kern_img.out > kern_img.asm
	objcopy -S -O binary kern_img.out kern_img

i386lib.o: i386lib.s 
	nasm -f elf -o i386lib.o i386lib.s

kernel.o: kern_start.c console.c kprintf.c i386lib.o keyboard.c 8259_pic.c
	gcc -o kernel.o -c -ffreestanding -nostdlib -nodefaultlibs -nostdinc -O0 kern_start.c \
		console.c kprintf.c keyboard.c 8259_pic.c

c.img: bootloader kern_img bootblock
	dd if=bootblock count=1 of=c.img conv=notrunc
	dd if=bootloader count=`./count_blocks bootloader 512` seek=1 of=c.img conv=notrunc
	dd if=kern_img count=`./count_blocks kern_img 512` seek=128 of=c.img conv=notrunc