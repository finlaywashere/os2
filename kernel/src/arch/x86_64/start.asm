bits 64

;section .bss
;align 8
;stack_bottom:
;resb 0x10000 ; 1MiB
;stack_top:

section .text
global _start
_start:
	mov rax, 0xB8000 ; Move video memory to rax
	mov byte [rax], 'A' 
