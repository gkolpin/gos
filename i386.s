extern kern_start
global START
	
BITS	32
[section .text]

START:
	cli
	mov	esp, START
	call	kern_start
