;;; Assembly routines for kernel

; outb (port, uint8 data)
	
[section .text]
	
global	outb

outb:
	push	ebp
	mov	ebp, esp
	push	edx
	push	eax
	mov	dx, [ebp + 8]	; port number
	mov	al, [ebp + 12]	; value
	out	dx, al
	pop	eax
	pop	edx
	pop	ebp
	ret