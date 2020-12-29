extern idt_ptr
global load_idt
load_idt:
	lidt [idt_ptr]
	sti
	ret

global enable_interrupts
enable_interrupts:
	sti
	ret
global disable_interrupts
disable_interrupts:
	cli
	ret

%macro push_all 0
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
%endmacro
	
%macro pop_all 0
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
%endmacro

isr_common:
	push_all
	
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	cld ; DF must be cleared before calling function
	
	extern isr_handler
	call isr_handler
	
	pop_all
	add rsp, 16 ; Skip 2 64 bit ints (error code, and irq number)
	iretq
%macro irq 2
global irq%1
irq%1:
	push byte 0
	push byte %2
	jmp isr_common
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

%macro ISR_NOERRORCODE 1
global isr%1
isr%1:
	push byte 0
	push byte %1
	jmp isr_common
%endmacro

%macro ISR_ERRORCODE 1
global isr%1
isr%1:
	push byte %1
	jmp isr_common
%endmacro

ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE 8
ISR_NOERRORCODE 9
ISR_ERRORCODE 10
ISR_ERRORCODE 11
ISR_ERRORCODE 12
ISR_ERRORCODE 13
ISR_ERRORCODE 14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_NOERRORCODE 17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31
