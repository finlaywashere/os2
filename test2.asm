bits 64

section .text

global _start
_start:
	mov rax, 1
	mov rbx, text2data
	mov rcx, 12
	int 80
	jmp $

text2data db 'Tset56784321'
