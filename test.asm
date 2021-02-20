bits 64

section .text

global _start
_start:
	mov rax, 1
	mov rbx, textdata
	mov rcx, 8
	int 80
	jmp $

textdata db 'Test1234'
