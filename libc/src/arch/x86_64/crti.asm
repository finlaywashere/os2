section .init
global _init
_init:
	push rbp
	mov rbp, rsp
	; Gcc will put crtbegin.o here

section .fini
global _fini
_fini:
	push rbp
	mov rbp, rsp
