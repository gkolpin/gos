;;; Assembly routines for kernel

;; void outb (uint16 port, uint8 data)
;; uint8 inb(uint16 port);
;; void outs (uint16 port, word_t* data, uint32 size);
;; void ins (uint16 port, word_t* buf, uint32 size);
;; void cmd_lidt(descriptor*)
;; void cmd_litr(uint16 tss_pos);
;; void cmd_sti();
;; void cmd_hlt();
;; void restart_task();
;; void load_cr3(uint32 pdb_addr);
;; void enable_paging();
;; uint32 bit_scan_forward(uint32)

BITS	32
[section .text]
	
extern	kb_int
extern	timer_int
extern	clock_tick
extern	cur_task_p
extern  pic_eoi
extern	ksyscall
extern	handle_page_fault
extern	tss_p
	
;; port I/O
global	outb
global	inb
global  ins
global	outs
;; instruction wrappers
global	cmd_lidt
global	cmd_sti
global	cmd_ltr
global	cmd_hlt
global  load_cr3
global	enable_paging
;; proc
global	restart_task
global	idle_task_start
global	GOS_BOTTOM_STACK
;; misc
global	get_eflags
global	bit_scan_forward
;; IDT
global 	IDT
global 	KEYBOARD_INT
global	TIMER_INT
global	SYSCALL_INT
global	GEN_INT
global	ERR_INT
global	DOUBLE_FAULT_INT
global	INVALID_TSS_INT
global	SEGMENT_NP_FAULT_INT
global	STACK_FAULT_INT
global	GENERAL_PROT_FAULT_INT
global	PAGE_FAULT_INT
global	LIDTR	

;; void outb(uint16 port, uint8 data);
outb:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]	; port number
	mov	al, [ebp + 12]	; value
	out	dx, al
	pop	ebp
	ret

;; uint8 inb(uint16 port);
inb:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]
	in	al, dx
	pop	ebp
	ret
	
;; void outs(uint16 port, word_t* data, uint32 size);
outs:
	push	ebp
	mov	ebp, esp
	push	esi		; save esi
	cld			; clear direction flag (ESI gets incremented)
	mov	ecx, [ebp + 16]	; number of times to repeat outsw command
	mov	esi, [ebp + 12]	; data address
	mov	dx, [ebp + 8]	; port
	
    rep outsw			; repeat outsw ecx times

	pop	esi
	pop	ebp
	ret
	
;; void ins (uint16 port, word_t* buf, uint32 size);
ins:
	push	ebp
	mov	ebp, esp
	push	edi		; save edi
	cld			; clear direction flag (EDI gets incremented)
	mov	ecx, [ebp + 16]	; number of times to repeat insw command
	mov	edi, [ebp + 12]	; buffer address
	mov	dx, [ebp + 8]	; port
	
    rep insw			; repeat insw ecx times

	pop	edi
	pop	ebp
	ret


;; void cmd_lidt(descriptor*)
;; load interrupt descriptor table pseudo-descriptor
cmd_lidt:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp + 8]
	lidt	[eax]	; memory operand
	pop	ebp
	ret
	
;; void cmd_ltr(uint16 tss_pos);
;; load tss descriptor position within gdt
cmd_ltr:
	push	ebp
	mov	ebp, esp
	mov	dx, [ebp + 8]	; tss descriptor location
	ltr	dx		; load tss register
	pop	ebp
	ret
	
;; void cmd_sti();
;; restore interrupts
cmd_sti:
	sti
	ret
	
;; void cmd_hlt();
;; halt the processor while waiting for interrupts
cmd_hlt:
	hlt
	ret
	
;; void load_cr3(uint32 base_addr);
;; load register cr3 with the page directory base address
load_cr3:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp + 8]
	mov	cr3, eax
	pop	ebp
	ret
	
;; void enable_paging();
;; set bit 31 of CR0 to enable paging
enable_paging:
	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax
	ret
	
;; uint32 bit_scan_forward(uint32)
;; bit scan forward
bit_scan_forward:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp + 8]
	bsf	eax, eax
	pop	ebp
	ret

;; uint32 get_eflags();
;; returns the current eflags
get_eflags:
	pushf
	mov	eax, [esp]
	add	esp, 4
	ret

%define	STACK_TOP	68
%define TSS_ESP0	4
%define	TSS_SS0		8

	
;; restart_task();
;; runs task pointed to by cur_task_p
restart_task:
	push	ebp
	mov	ebp, esp
		
	mov	eax, [cur_task_p]
	add	eax, STACK_TOP
	mov	ecx, [tss_p]
	mov	[ecx + TSS_ESP0], eax

	mov	esp, [cur_task_p]
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	iretd
	
idle_task_start:
	sti
	hlt
	cli
	jmp	idle_task_start
	

ALIGN 8				; align IDT to 8 byte boundary
IDT:
	TIMES 0x800 DB 0	; fill the 256 * 8 byte table with zeros
	
%macro	KERN_ENTER 0
	pusha
	push	ds
	push	es
	push	fs
	push	gs
	
	mov	esp, GOS_BOTTOM_STACK
%endmacro

KEYBOARD_INT:
	KERN_ENTER
	call 	pic_eoi
	call	kb_int		;call kb interrupt handler in tty.c
	call 	restart_task
	
TIMER_INT:
	KERN_ENTER
	call	pic_eoi		; controller eoi
	call	clock_tick	; call clock_tick of clock driver
				; at each timer tick
	call	restart_task
	
hang: 
	jmp	hang
	
SYSCALL_INT:
	KERN_ENTER
	push	edx		; syscall parameters were passed in regiters
	push	ecx
	push	ebx
	push	eax
	call	ksyscall
	add	esp, 16		; pop 4 syscall parameters off stack
	
	call	restart_task
	
GEN_INT:
	iretd
	
ERR_INT:
	add	esp, 4		; remove error code
	iretd

DOUBLE_FAULT_INT:
	add	esp, 4		; remove error code
	iretd
	
INVALID_TSS_INT:
	add	esp, 4
	iretd

SEGMENT_NP_FAULT_INT:
	add	esp, 4
	iretd

STACK_FAULT_INT:
	add	esp, 4		; remove error code
	iretd

GENERAL_PROT_FAULT_INT:
	add	esp, 4
	iretd

PAGE_FAULT_INT:
	pop	dword [ERR_NUM]
	KERN_ENTER
	push	dword [ERR_NUM]
	call	handle_page_fault ; error code has been pushed on stack
	add	esp, 4
	call	restart_task
	
ALIGN 8
	DW	0		; align LIDT pseudo-descriptor to odd word
LIDTR:
	DW	0x7FF		; 256 * 8 - 1 :	 size of idt
	DD	IDT

GOS_TOP_STACK:
	TIMES	0x1000 DB 0
GOS_BOTTOM_STACK:

[section .data]
ERR_NUM:	DD 0