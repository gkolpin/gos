ORG 1000h
	
%define	LOADOFF	0x1000
	
%define	PE_ON		0x1
%define	CODE_SELECTOR	0x8
%define DATA_SELECTOR	0x10
%define	STACK_SELECTOR	0x18

BITS	16
[section .text]
	
START:
	
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
	mov	ah, 0x42	; load kernel image into memory
	mov	dl, 0x80
	mov	bx, DAP
	mov	si, bx
	int	0x13
	
real2prot:
	cli
	xor	ax, ax
	mov	ds, ax
	lgdt	[GDT_DESCR]
	
	mov	eax,  cr0
	or	eax,  PE_ON
	mov	cr0,  eax
	jmp	dword CODE_SELECTOR:prot_mode_start
	
BITS 32
prot_mode_start:
	mov	ax, DATA_SELECTOR
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	
	mov	esp, START
	
	sti

	jmp	0x10000		; kernel loaded at 0x10000

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

	DB	0x00		; segment 0x1000
	DB	0x00
	DB	0x00		;  offset 0x00
	DB	0x10

	DB	0x80		; absolute sector number to read from
	DB	0x00		; kernel will start at block 0x80
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
	DB	0x00
ENDDAP:	
		
align	8			; align global descriptor table to 8 byte boundary

GDT:
	
NULL_DESCR:	
	DW	0x00		; null descriptor
	DW	0x00
	DW	0x00
	DW	0x00

CODE_DESCR:
	DW	0xffff		; limit
	DW	0x0000		; base = 0
	DB	0x00		; base = 0
	DB	0x9A
	DB	0xCF		; flags = C, limit = F
	DB	0x00		; base = 0

DATA_DESCR:
	DW	0xffff		; limit
	DW	0x0000		; base = 0
	DB	0x00		; base = 0
	DB	0x92
	DB	0xCF		; flags = C, limit = F
	DB	0x00		; base = 0
	
STACK_DESCR:			; grows downwards
	DW	0xffff
	DW	0x0000
	DB	0x00
	DB	0x96
	DB	0xCF
	DB	0x00
	
	DW	0x00		; pseudo-descriptor needs to be on odd word address
GDT_DESCR:
	DW	0x17
	DD	GDT
	