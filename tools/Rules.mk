# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

CLEAN	:= ${CLEAN} ${d}/patch_boot ${d}/count_blocks ${d}/fs_format

${d}/patch_boot: ${d}/patch_boot.c
	gcc -o $@ $<

${d}/count_blocks: ${d}/count_blocks.c
	gcc -o $@ $<

${d}/fs_format: ${d}/fs_format.c
	gcc -o $@ $<

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
