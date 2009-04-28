# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

SRC_${d} 	:= ${wildcard ${d}/*.c}
OBJ_${d}	:= ${SRC_${d}:.c=.o}
DEP_${d}	:= ${OBJ_${d}:.o=.d}

CLEAN	:= ${CLEAN} ${OBJ_${d}} ${DEP_${d}} ${d}/at_hdd.a
LIBS	:= ${LIBS} ${d}/at_hdd.a

${OBJ_${d}}: 	CF_DIR := -iquote ${d}
${DEP_${d}}: 	DEP_DIR := ${d}
${d}/at_hdd.a:	${OBJ_${d}}
	${AR}


include ${DEP_${d}}

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
