# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

SRC_${d} 	:= ${wildcard ${d}/*.c}
OBJ_${d}	:= ${SRC_${d}:.c=.o}
DEP_${d}	:= ${OBJ_${d}:.o=.d}

CLEAN	:= ${CLEAN} ${OBJ_${d}} ${DEP_${d}} ${d}/simple_fs.a
LIBS	:= ${LIBS} ${d}/simple_fs.a


${OBJ_${d}}: 	CF_DIR := -iquote ${d}
${DEP_${d}}: 	DEP_DIR := ${d}
${d}/simple_fs.a:	${OBJ_${d}}
	${AR}

include ${DEP_${d}}

${DEP_${d}}: 	${SRC_${d}}
	${DEPEND} ${DEP_DIR} ${CF_GLOBAL} $< > $@

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
