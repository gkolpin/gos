SRC_FILES=kern_start.c console.c kprintf.c keyboard.c 8259_pic.c\
	intvect.c ksignal.c mm.c prot.c sched.c syscall.c utility.c \
	task.c at_disk_driver.c simple_fs.c vm.c kmalloc.c \
	elf_loader.c fork.c kill.c clock.c uptime.c exit.c waitpid.c

OBJ_FILES=kern_start.o console.o kprintf.o keyboard.o 8259_pic.o\
	 intvect.o ksignal.o mm.o prot.o sched.o syscall.o utility.o \
	 task.o at_disk_driver.o simple_fs.o vm.o kmalloc.o \
	elf_loader.o fork.o kill.o clock.o uptime.o exit.o waitpid.o

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

kern_img: i386.s ${OBJ_FILES} i386lib.o testproc_lib.o
	nasm -f elf -o i386.o i386.s
	ld -melf_i386_fbsd -N -e START -Ttext 0x10000 -o kern_img.out i386.o ${OBJ_FILES} i386lib.o \
		testproc_lib.o
	objdump	-S kern_img.out > kern_img.asm
	objcopy -S --pad-to=0x1FE00 -O binary kern_img.out kern_img

i386lib.o: i386lib.s 
	nasm -f elf -o i386lib.o i386lib.s

testproc_lib.o: testproc_lib.s
	nasm -f elf -o testproc_lib.o testproc_lib.s

${OBJ_FILES}: ${SRC_FILES}
	gcc -c -ffreestanding -nostdlib -nodefaultlibs -nostdinc -fpack-struct -O0 -m32 -march=i386 ${SRC_FILES}

testproc.o: testproc.c syscall.h
	gcc -o testproc.o -c -m32 -march=i386 testproc.c

testproc: testproc.o testproc_lib.o
	ld -melf_i386_fbsd -N -e proc1 -Ttext 0x200000 -o testproc testproc.o testproc_lib.o

fs_index: fs_format testproc
	./fs_format fs_index `./count_blocks testproc 512`

c.img: bootloader kern_img bootblock fs_index testproc
	dd if=bootblock count=1 of=c.img conv=notrunc
	dd if=bootloader count=`./count_blocks bootloader 512` seek=1 of=c.img conv=notrunc
	dd if=kern_img count=`./count_blocks kern_img 512` seek=128 of=c.img conv=notrunc
	cat fs_index testproc > fs.img
	dd if=fs.img count=`./count_blocks fs.img 512` seek=1000 of=c.img conv=notrunc
