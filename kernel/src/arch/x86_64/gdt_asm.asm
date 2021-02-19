extern gdt_ptr
global load_gdt
load_gdt:
        lgdt [gdt_ptr]
	push rax
	mov rax, 0x28
	ltr ax
	pop rax
        ret
