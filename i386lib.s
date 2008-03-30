;;; Assembly routines for kernel

; outb (port, uint8 data)
	
BITS	32
[section .text]
	
extern	kb_int
	
global	outb
global	inb
global 	IDT
global 	KEYBOARD_INT
global	cmd_lidt
global	cmd_sti
global	cmd_int
global	GEN_INT
global	ERR_INT
global	LIDTR
global	cmd_hlt

outb:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]	; port number
	mov	al, [ebp + 12]	; value
	out	dx, al
	pop	ebp
	ret


inb:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]
	in	al, dx
	pop	ebp
	ret
	
cmd_lidt:
	lidt	[LIDTR]	; memory operand
	ret
	
	;; restore interrupts
cmd_sti:
	sti
	ret
	
cmd_int:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp + 8]
	int	51
	pop	ebp
	ret
	
cmd_hlt:
	hlt
	ret

ALIGN 8				; align IDT to 8 byte boundary
IDT:
	TIMES 0x800 DB 0	; fill the 256 * 8 byte table with zeros

KEYBOARD_INT:
	cli
	pushad
	call	kb_int
	popad
	sti
	iretd

GEN_INT:
	iretd
	
ERR_INT:
	add	esp, 4		; remove error code
	iretd
	
LIDTR:
	DW	0x7FF		; 256 * 8 - 1 :	 size of idt
	DD	IDT

