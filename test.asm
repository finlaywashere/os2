bits 64

section .text

global _start
_start:
	mov rax, 3
	int 80
	cmp rax, 0
	jg .child
	mov rax, 1
	mov rbx, textdata
	mov rcx, 8
	int 80
	jmp $
.child:
	mov rax, 1
        mov rbx, text2data
        mov rcx, 8
        int 80
	jmp $

textdata db 'Test1234'
text2data db 'Tset4321'
