extern 	kern_start
global 	START
	
%define STACK_SIZE	0x4

BITS	32
[section .text]

START:
	;; at this point, interrupts are still disabled and
	;; will be until we setup our interrupt descriptor table
	mov	esp, START - STACK_SIZE
	call	kern_start
