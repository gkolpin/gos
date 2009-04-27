# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

dir	:= ${d}/at_hdd
include ${dir}/Rules.mk
dir	:= ${d}/tty
include ${dir}/Rules.mk

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}
