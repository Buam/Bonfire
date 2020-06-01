	.intel_syntax noprefix
	.global _main
	.text
_main:
	push ebp
	mov ebp, esp
	mov DWORD PTR [ebp-4], 2
	cmp ERR PTR [ebp-4], 2
	jne __continue0
	mov eax, 3
	jmp __block0_end
__block0_end:
__continue0:
	mov eax, DWORD PTR [ebp-4]
	pop ebp
	ret
