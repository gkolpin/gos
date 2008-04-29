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

%define	stack_p	8

restart_task:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp + 8]
	mov	esp, [eax + stack_p]
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
	pushad
	mov	eax, [cur_task_p]
	mov	[eax + stack_p], esp
	call	pic_eoi		; controller eoi
	call	sched_int	; call sched_int at each timer tick
	popad			; we'll never actually get here, 
				; as the task will have been switched and started in the call to sched_int
	sti
	iretd
	
SYSCALL_INT:
	cli
	pushad
	push	edx		; syscall parameters were passed in regiters
	push	ecx
	push	ebx
	push	eax
	call	ksyscall
	add	esp, 16		; pop 4 syscall parameters off stack
	popad
	sti
	iretd
	
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

