# Build flags
SYS_HEADERS	:= sys_headers
CF_GLOBAL	:= -g -I${SYS_HEADERS} -ffreestanding -nostdlib -nodefaultlibs -nostdinc -fpack-struct -O0 -m32 -march=i386
ASMF_GLOBAL	:= -f elf
LDF_GLOBAL	:= -melf_i386_fbsd -N

# Build tools
CC		:= gcc
COMP		= ${CC} ${CF_GLOBAL} ${CF_DIR} -c -o $@ $<
DEPEND		:= ./depend.sh
DEP_BUILD	= ${DEPEND} ${dir $*} ${CF_GLOBAL} $*.c > $@
AR		= ar -c -r $@ $^
ASM		= nasm ${ASMF_GLOBAL} -o $@ $<
LD		= ld

include Rules.mk