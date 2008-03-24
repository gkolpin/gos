

patch_boot: patch_boot.c
	gcc -o patch_boot patch_boot.c

count_blocks: count_blocks.c
	gcc -o count_blocks count_blocks.c

bootloader: bootloader.s patch_boot progsect2 count_blocks
	nasm -f bin bootloader.s
	./patch_boot bootloader `./count_blocks progsect2 512`

progsect2: progsect2.s kernel.o i386lib.o
	nasm -f elf -o progsect2.o progsect2.s
	ld -N -e START -Ttext 0x1000 -o progsect2.out progsect2.o kernel.o i386lib.o
	objdump	-S progsect2.out > progsect2.asm
	objcopy -S -O binary progsect2.out progsect2

i386lib.o: i386lib.s 
	nasm -f elf -o i386lib.o i386lib.s

kernel.o: kern_start.c console.c kprintf.c i386lib.o keyboard.c
	gcc -o kernel.o -c -ffreestanding -nostdlib -nodefaultlibs -nostdinc -O0 kern_start.c \
		console.c kprintf.c keyboard.c

c.img: bootloader progsect2
	dd if=bootloader count=1 of=c.img conv=notrunc
	dd if=progsect2 count=`./count_blocks progsect2 512` seek=1 of=c.img conv=notrunc