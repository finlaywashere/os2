extern idt_ptr
global load_idt
load_idt:
	lidt [idt_ptr]
	sti
	ret

push_all:
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsp
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, cr2
	push rax
	mov rax, ds
	push rax
	mov rax, ss
	push rax
	mov rax, es
	push rax
	mov rax, fs
	push rax
	mov rax, gs
	push rax

pop_all:
	pop rax
	mov gs, ax
	pop rax
	mov fs, ax
	pop rax
	mov es, ax
	pop rax
	mov ss, ax
	pop rax
	mov ds, ax
	pop rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsp
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax

irq_common:
	call push_all
	
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	extern irq_handler
	call irq_handler
	
	call pop_all
	add rsp, 16 ; Skip 2 64 bit ints (error code, and irq number)
	iretq
%macro irq 2
global irq%1
irq%1:
	push byte 0 
	push byte %2
	jmp irq_common
%endmacro

irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35 
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

