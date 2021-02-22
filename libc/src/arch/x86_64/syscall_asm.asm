global syscall
syscall:
	push rbx
	
	mov rax, rdi
	mov rbx, rsi
	push r8
	mov r8, rdx
	mov rdx, rcx
	mov rcx, r8
	pop r8
	int 80
	pop rbx
	ret
