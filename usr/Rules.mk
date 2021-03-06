# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

SRC_${d} 	:= ${wildcard ${d}/*.c}
ASM_SRC_${d}	:= ${wildcard ${d}/*.s}
OBJ_${d}	:= ${d}/testproc.o ${SRC_${d}:.c=.o} ${ASM_SRC_${d}:.s=.o}
DEP_${d}	:= ${SRC_${d}:.c=.d}


CLEAN	:= ${CLEAN} ${OBJ_${d}} ${DEP_${d}} ${d}/testproc ${d}/testproc.asm


${OBJ_${d}}: 	CF_DIR := -iquote ${d}
${DEP_${d}}: 	DEP_DIR := ${d}
${d}/testproc.asm:	${d}/testproc
	objdump -S $< > $@
${d}/testproc:	${OBJ_${d}}
	${LD} ${LDF_GLOBAL} -e proc1 -Ttext 0x200000 -o $@ $^


include ${DEP_${d}}

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
