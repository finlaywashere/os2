bits 64

section .bss
align 8
stack_bottom:
resb 0x10000 ; 1MiB
stack_top:

section .text
global _start
_start:
	pop rbx
	mov rsp, stack_top ; Setup stack
	push rbx
	
	mov ax, 16
	
	; Setup segments
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ax, 0
	
	extern kernel_start
	call kernel_start
	
	; Something terrible has happened
	cli
.hang: hlt
	jmp .hang
