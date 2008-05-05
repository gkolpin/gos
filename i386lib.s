;;; Assembly routines for kernel

; outb (port, uint8 data)
	
BITS	32
[section .text]
	
extern	kb_int
extern	timer_int
extern	sched_int
extern	cur_task_p
extern  pic_eoi
extern	ksyscall
extern	tss_p
	
global	outb
global	inb
global 	IDT
global 	KEYBOARD_INT
global	TIMER_INT
global	SYSCALL_INT
global	MAKE_SYSCALL
global	cmd_lidt
global	cmd_sti
global	cmd_int
global	cmd_ltr
global	GEN_INT
global	ERR_INT
global	LIDTR
global	cmd_hlt
global	restart_task
global	get_eflags
global	GOS_BOTTOM_STACK

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
	
cmd_ltr:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]	; tss descriptor location
	ltr	dx		; load tss register
	pop	ebp
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
	
get_eflags:
	pushf
	mov	eax, [esp]
	add	esp, 4
	ret

%define	STACK_TOP	68
%define TSS_ESP0	4
%define	TSS_SS0		8

	
;; restart_task(task*);
restart_task:
	push	ebp
	mov	ebp, esp
	
	mov	eax, [cur_task_p]
	add	eax, STACK_TOP
	mov	ecx, [tss_p]
	mov	[ecx + TSS_ESP0], eax

	mov	esp, [ebp + 8]
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	sti
	iretd
	

ALIGN 8				; align IDT to 8 byte boundary
IDT:
	TIMES 0x800 DB 0	; fill the 256 * 8 byte table with zeros

KEYBOARD_INT:
	cli
	pushad
	call	kb_int
	call	pic_eoi		; controller eoi
	popad
	sti
	iretd
	
TIMER_INT:
	cli
	pusha
	push	ds
	push	es
	push	fs
	push	gs
	
	mov	esp, GOS_BOTTOM_STACK
	
	call	pic_eoi		; controller eoi
	call	sched_int	; call sched_int at each timer tick
				; we'll never actually get here, 
				; as the task will have been switched and started in the call to sched_int
hang: 
	jmp	hang
	
SYSCALL_INT:
	cli
	pusha
	push	ds
	push	es
	push	fs
	push	gs
	
	mov	esp, GOS_BOTTOM_STACK
	
	push	edx		; syscall parameters were passed in regiters
	push	ecx
	push	ebx
	push	eax
	call	ksyscall
	add	esp, 16		; pop 4 syscall parameters off stack
	
	push	dword [cur_task_p]
	call	restart_task
	
MAKE_SYSCALL:
	push	ebp
	mov	ebp, esp
	push	ebx		; callee saved register
	mov	edx, [ebp + 20]	; pass parameters on the registers
	mov	ecx, [ebp + 16]
	mov	ebx, [ebp + 12]
	mov	eax, [ebp + 8]
	int	50		; make syscall
	pop	ebx
	pop	ebp
	ret

GEN_INT:
	iretd
	
ERR_INT:
	add	esp, 4		; remove error code
	iretd
	
ALIGN 8
	DW	0		; align LIDT pseudo-descriptor to odd word
LIDTR:
	DW	0x7FF		; 256 * 8 - 1 :	 size of idt
	DD	IDT

GOS_TOP_STACK:
	TIMES	0x1000 DB 0
GOS_BOTTOM_STACK:	