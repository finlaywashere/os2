bits 64

global _start
_start:
	mov rax, 1
	mov rbx, 'A'
	int 80
	jmp $
