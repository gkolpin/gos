

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

kern_img: i386.s kernel.o i386lib.o testproc_lib.o
	nasm -f elf -o i386.o i386.s
	ld -N -e START -Ttext 0x10000 -o kern_img.out i386.o kernel.o i386lib.o \
		testproc_lib.o
	objdump	-S kern_img.out > kern_img.asm
	objcopy -S --pad-to=0x1FE00 -O binary kern_img.out kern_img

i386lib.o: i386lib.s 
	nasm -f elf -o i386lib.o i386lib.s

testproc_lib.o: testproc_lib.s
	nasm -f elf -o testproc_lib.o testproc_lib.s

kernel.o: kern_start.c console.c kprintf.c i386lib.o keyboard.c 8259_pic.c\
	 intvect.c ksignal.c mm.c prot.c sched.c syscall.c utility.c testproc.c \
	testproc_lib.o task.c at_disk_driver.c simple_fs.c vm.c kmalloc.c \
	elf_loader.c fork.c kill.c clock.c
	gcc -o kernel.o -c -ffreestanding -nostdlib -nodefaultlibs -nostdinc -fpack-struct -O0 kern_start.c \
		console.c kprintf.c keyboard.c 8259_pic.c intvect.c ksignal.c mm.c \
		prot.c sched.c syscall.c utility.c testproc.c task.c at_disk_driver.c \
		simple_fs.c vm.c kmalloc.c elf_loader.c fork.c kill.c clock.c

testproc.o: testproc.c
	gcc -o testproc.o -c testproc.c

testproc: testproc.o testproc_lib.o
	ld -N -e proc1 -Ttext 0x200000 -o testproc testproc.o testproc_lib.o

fs_index: fs_format testproc
	./fs_format fs_index `./count_blocks testproc 512`

c.img: bootloader kern_img bootblock fs_index testproc
	dd if=bootblock count=1 of=c.img conv=notrunc
	dd if=bootloader count=`./count_blocks bootloader 512` seek=1 of=c.img conv=notrunc
	dd if=kern_img count=`./count_blocks kern_img 512` seek=128 of=c.img conv=notrunc
	cat fs_index testproc > fs.img
	dd if=fs.img count=`./count_blocks fs.img 512` seek=1000 of=c.img conv=notrunc
