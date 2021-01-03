bits 64
global _start
_start:
	mov rax, 'A'
	mov rbx, 0xffffff80000b8000
	mov [rbx], rax
	jmp $
