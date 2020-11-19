bits 16

org 0x7C00
section .text

global _start
_start:
	; Setup page tables now to save trouble later
	mov edx, 0x8000
        mov ebx, 0x8003
        mov [edx], ebx
	mov ecx, 1
.loop:
	mov eax, 0x1000 ; 1KiB
	mul ecx
	or eax,0b11
	mov [0x8000 + ecx*8], eax ; map entry
	inc ecx
	cmp ecx, 512
	jne .loop
	
	; Load kernel from disk now so we can use the BIOS
	mov si, read_command
	mov ah, 0x42
	mov dl, 0x80 ; Disk number
	int 0x13
	
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
        mov ecx, 0xC0000080
        rdmsr
        or eax, 1 << 8
        wrmsr
	
	; Enable paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	; Load long mode GDT
        xor eax, eax
        mov ax, ds
        shl eax, 4
        add eax, GDT2
        mov [gdtr2 + 2], eax
        mov eax, GDT2_end
        sub eax, GDT2
        mov [gdtr2], ax
        lgdt [gdtr2]
	
	jmp 08h:longMode
bits 64
longMode:
	mov rax, 0x10
	mov ds, rax
	mov ss, rax
	mov es, rax
	mov fs, rax
	mov gs, rax
	jmp $
section .rodata
read_command:
	db 16 ; Packet size
	db 0 ; Zero
	dw 16 ; Number of sectors to transfer
	dw 0x10 ; Data segment for transfer buffer
	dw 0x9000 ; Transfer buffer address
	dd 1 ; Lower 32 bits of LBA
	dd 0 ; Upper 32 bits of LBA
gdtr:
	dw 0
	dd 0
GDT:
	dq 0 ; null segment
	
	dw 0xFFFF ; Code limit
	dw 0 ; Code base low
	db 0 ; Code base mid
	db 10011000b ; Code access
	db 11001111b ; Code flags
	db 0 ; Code base high

	dw 0xFFFF ; Data limit
        dw 0 ; Data base low
        db 0 ; Data base mid
        db 10010010b ; Data access
        db 10001111b ; Data flags
        db 0 ; Data base high
GDT_end:
gdtr2:
        dw 0
        dd 0
GDT2:
        dq 0 ; null segment

        dw 0xFFFF ; Code limit
        dw 0 ; Code base low
        db 0 ; Code base mid
        db 10011000b ; Code access
        db 10101111b ; Code flags
        db 0 ; Code base high

        dw 0xFFFF ; Data limit
        dw 0 ; Data base low
        db 0 ; Data base mid
        db 10010010b ; Data access
        db 10001111b ; Data flags
        db 0 ; Data base high

GDT2_end:
	resb 202
	db 0x55
	db 0xAA
	dq 0xFFFFFFFF
	dq 0xFFFFFFFF
