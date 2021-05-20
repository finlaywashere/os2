bit 64

global check_cpuid
check_cpuid:
	pushfq ; Save rflags
	pushfq
	xor qword [rsp], 0x200000 ; Set bit 22
	popfq ; Set rflags
	pushfq ; See if bit 22 is set
	pop rax
	xor rax, [rsp]
	popfq
	and rax, 0x200000
	ret ; Returns 0 if cpuid is not supported

global cpuid
cpuid:
	mov rax, rdi
	cpuid
	mov [rsi], ebx
	add rsi, 4
	mov [rsi], edx
	add rsi, 4
	mov [rsi], ecx
	ret
