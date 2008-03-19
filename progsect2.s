; ORG	1000h
	
%define	PE_ON		0x1
%define	CODE_SELECTOR	0x8
%define DATA_SELECTOR	0x10
%define	STACK_SELECTOR	0x18
	
extern kern_start
global START
	
BITS 16
	
[section .text]

START:

enable_A20_1:			; enable A20 address line
	in	al, 0x64	; busy wait until keyboard not busy
	test	al, 0x2
	jnz	enable_A20_1

	mov	al, 0xd1	; send data to port
	out	0x64, al

enable_A20_2:
	in	al, 0x64	; busy wait
	test	al, 0x2
	jnz	enable_A20_1

	mov	al, 0xdf	; send data to port
	out	0x60, al	


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
	
	call	kern_start

MSG:
	DB "Loaded second prog "
ENDMSG:
	
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
	