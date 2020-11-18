bits 16

org 0x7C00
section .text

global _start
_start:
	mov ah, 0xe
	mov al, 'T'
	int 0x10
	jmp _start

section .rodata
        resb 502
	db 0x55
	db 0xAA
