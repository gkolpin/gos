;; Assembly routines for processes

BITS	32

global	MAKE_SYSCALL

;; void MAKE_SYSCALL(uint32 syscall_number, uint32 p1, uint32 p2, uint32 p3);
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
