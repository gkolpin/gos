SRC_FILES=kern_start.c console.c kprintf.c keyboard.c 8259_pic.c\
	intvect.c ksignal.c mm.c prot.c sched.c syscall.c utility.c \
	task.c at_disk_driver.c simple_fs.c vm.c kmalloc.c \
	elf_loader.c fork.c kill.c clock.c uptime.c exit.c waitpid.c \
	brk.c pf.c getpid.c devices.c vfs.c devfs.c list.c tty.c

HEADER_FILES=*.h

OBJ_FILES=kern_start.o console.o kprintf.o keyboard.o 8259_pic.o\
	 intvect.o ksignal.o mm.o prot.o sched.o syscall.o utility.o \
	 task.o at_disk_driver.o simple_fs.o vm.o kmalloc.o \
	elf_loader.o fork.o kill.o clock.o uptime.o exit.o waitpid.o \
	brk.o pf.o getpid.o devices.o vfs.o devfs.o list.o tty.o

%.o	: %.c
	${COMP}

%.o	: %.s
	${ASM}

TOOLS_D		:= tools
KERN_IMG	:= kern/kern_img

dir 	:=tools
include ${dir}/Rules.mk
dir	:=boot
include ${dir}/Rules.mk
dir	:=fs
include ${dir}/Rules.mk
dir	:=drivers
include ${dir}/Rules.mk
dir	:=kern
include ${dir}/Rules.mk
dir	:=usr
include ${dir}/Rules.mk

fs_index: tools/fs_format usr/testproc tools/count_blocks
	tools/fs_format fs_index `${TOOLS_D}/count_blocks usr/testproc 512`

fs.img: fs_index usr/testproc
	cat fs_index usr/testproc > fs.img

c.img: boot/bootloader kern/kern_img boot/bootblock fs.img usr/testproc ${TOOLS_D}/count_blocks
	dd if=boot/bootblock count=1 of=c.img conv=notrunc
	dd if=boot/bootloader count=`${TOOLS_D}/count_blocks boot/bootloader 512` seek=1 of=c.img conv=notrunc
	dd if=kern/kern_img count=`${TOOLS_D}/count_blocks kern/kern_img 512` seek=128 of=c.img conv=notrunc
	dd if=fs.img count=`${TOOLS_D}/count_blocks fs.img 512` seek=1000 of=c.img conv=notrunc

.PHONY: clean
clean: 
	rm -f ${CLEAN} fs_index fs.img
