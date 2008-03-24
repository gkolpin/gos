;;; Assembly routines for kernel

; outb (port, uint8 data)
	
[section .text]
	
global	outb
global	inb

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
	mov	dx, [ebp + 8]	; port number
	in	al, dx
	pop	ebp
	ret