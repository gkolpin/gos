extern kern_start
global START
	
BITS	32
[section .text]

START:
	;; at this point, interrupts are still disabled and
	;; will be until we setup our interrupt descriptor table
	mov	esp, START
	call	kern_start
