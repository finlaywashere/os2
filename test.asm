bits 64

section .text

global _start
_start:
	mov rax, 3
	int 80
	cmp rax, 0
	je .child
	mov rax, 1
	mov rbx, textdata
	mov rcx, 8
	int 80
	jmp $
.child:
	mov rax, 4
	mov rbx, text2data
	int 80
	jmp $

textdata db 'Test1234'
text2data db '/test2.elf', 0x0
