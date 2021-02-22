section .text
global _start
_start:
	push rsi
	push rdi
	push rdx
	push rcx
	
	; Initialize libc
	extern initialize_standard_library
	call initialize_standard_library
	
	; Global contructors
	extern _init
	call _init
	
	pop rcx
	pop rdx
	pop rdi
	pop rsi
	
	; Call main function
	extern main
	call main
	
	; Move return code of main to the first argument to the next function to call
	mov rdi, rax
	extern exit
	call exit
