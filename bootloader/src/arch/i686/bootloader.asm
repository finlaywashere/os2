bits 16

org 0x7C00
section .text

global _start
_start:
	mov ecx, 0x8000
        mov ebx, 0x8003
        mov [ecx], ebx
	mov ebx, 0x7003
	mov [ecx+7*8], ebx
	mov ebx, 0x8003
	mov [ecx+8*8], ebx
	
	cli ; Disable interrupts
	; Quickly set A20
	in al, 0x92
	or al, 2
	out 0x92, al
	
	; Load GDT
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, GDT
	mov [gdtr + 2], eax
	mov eax, GDT_end
	sub eax, GDT
	mov [gdtr], ax
	lgdt [gdtr]
	
	; Enter protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax
	
	jmp 08h:protectedMode
bits 32
protectedMode:
	mov ecx, 0x8000
	
	mov cr3, ecx
		
	; Enable PAE
        mov eax, cr4 ; Copy CR4 to eax
        or eax, 100000b ; Enable the PAE bit
        mov cr4, eax ; Move changes to CR4
	
	; Set long mode bit in MSR
        ;mov ecx, 0xC0000080
        ;rdmsr
        ;or eax, 1 << 8
        ;wrmsr
	
	; Enable paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	jmp $
section .rodata
gdtr:
	dw 0
	dd 0
GDT:
	dq 0 ; null segment
	
	dw 0xFFFF ; Code limit
	dw 0 ; Code base low
	db 0 ; Code base mid
	db 10011010b ; Code access
	db 11001111b ; Code flags
	db 0 ; Code base high

	dw 0xFFFF ; Data limit
        dw 0 ; Data base low
        db 0 ; Data base mid
        db 10010010b ; Data access
        db 11001111b ; Data flags
        db 0 ; Data base high

GDT_end:
        resb 352
	db 0x55
	db 0xAA
