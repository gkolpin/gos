# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

SRC_${d} 	:= ${wildcard ${d}/*.c}
ASM_SRC_${d}	:= ${wildcard ${d}/*.s}
OBJ_${d}	:= ${d}/i386.o ${SRC_${d}:.c=.o} ${ASM_SRC_${d}:.s=.o}
DEP_${d}	:= ${SRC_${d}:.c=.d}

CLEAN	:= ${CLEAN} ${OBJ_${d}} ${DEP_${d}} ${d}/kern_img.asm \
		${d}/kern_img.out ${d}/kern_img

${OBJ_${d}}: 	CF_DIR := -iquote ${d}
${DEP_${d}}: 	DEP_DIR := ${d}
${d}/kern_img.out:	${OBJ_${d}} ${LIBS}
	${LD} ${LDF_GLOBAL} -e START -Ttext 0x10000 -o $@ $^
${d}/kern_img.asm: ${d}/kern_img.out
	objdump -S $< > $@
${d}/kern_img: ${d}/kern_img.out ${d}/kern_img.asm
	objcopy -S --pad-to=0x1FE00 -O binary $< $@

include ${DEP_${d}}

${DEP_${d}}: 	${SRC_${d}}
	${DEPEND} ${DEP_DIR} ${CF_GLOBAL} $< > $@

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
