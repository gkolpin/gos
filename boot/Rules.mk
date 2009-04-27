# save current directory, d
sp 		:= ${sp}.${dir}
dirstack_${sp}	:= ${d}
d 		:= ${dir}

CLEAN	:= ${CLEAN} ${d}/bootblock ${d}/bootloader
BOOTLOADER	:= ${d}/bootloader

${d}/bootblock: ${d}/bootblock.s ${TOOLS_D}/patch_boot ${d}/bootloader \
		${TOOLS_D}/count_blocks
	nasm -f bin -o $@ $<
	${TOOLS_D}/patch_boot $@ `${TOOLS_D}/count_blocks ${BOOTLOADER} 512`

${d}/bootloader: ${d}/bootloader.s ${TOOLS_D}/patch_boot ${KERN_IMG} \
		${TOOLS_D}/count_blocks
	nasm -f bin -o $@ $<
	${TOOLS_D}/patch_boot $@ `${TOOLS_D}/count_blocks ${KERN_IMG} 512`

# restore d
d		:= ${dirstack_${sp}}
sp		:= ${basename ${sp}}