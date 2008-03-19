ORG 7C00h
	
%define	LOADOFF	0x7C00

[section .text]
	
START:
	
	mov	ah, 0x06
	mov	al, 0x00
	mov	bh, 0x07
	mov	cx, 0x00
	mov	dh, 0x18
	mov	dl, 0x4f
	int	0x10		; clear screen
	
	mov	ah, 0x02
	mov	bh, 0x00
	mov	dx, 0x00
	int	0x10		; set cursor to upper left
	
	xor	ax, ax
	cli
	mov	ss, ax
	mov	sp, LOADOFF
	sti
	
	mov	bp, sp
	
load_opsys:	

	mov	ah, 0x41
	mov	dl, 0x80
	mov	bx, 0x55AA
	int	0x13
	
	jc	error
	
	cmp	bx, 0xAA55
	jnz	error

	test	CL, 0x01
	jz	error
	
load:
	mov	ah, 0x42
	mov	dl, 0x80
	mov	bx, DAP
	mov	si, bx
	int	0x13
	jmp	0x1000

hang:
	jmp	hang		; loop forever
	
	
error:	
	mov	si, MSG2
	push	ENDMSG2
	call	print
	add	sp, 2
	jmp	hang
	
print:	
	push	bp
	mov	bp, sp
print_loop:	
	lodsb	
	cmp	si, [bp+4]
	je	print_return
	
	mov	ah, 0x0E	; write text
	mov	bh, 0x00
	mov	bl, 0x01	; write white text
	int	0x10
	jmp	print_loop
	
print_return:
	pop	bp
	ret
	

MSG2:
	DB "Error, bios doesn't support extended disk reads "
ENDMSG2:	

DAP:
	DB	0xDE
	DB	0xAD
	DB	0xBE
	DB	0xEF

	DB	0x00
	DB	0x00
	DB	0x00		;  place at address 0x1000
	DB	0x01

	DB	0x01		; absolute sector number to read from
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
ENDDAP:	

TIMES 0200h - 2 - ($ - $$) DB 0 ;Zerofill up to 510 bytes

DW 0AA55h ;Boot Sector signature