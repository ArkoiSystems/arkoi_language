.intel_syntax noprefix
.file 1 "factorial.ark"

.section .text
.global _start
_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

.global main
.type main, @function
main:
	enter 16, 0
	# %01 @u32 = alloca
	# arg @u32 7
	# $05 @u32 = call factorial, 1
	.loc 1 2 0
	mov edi, 7
	call factorial
	# store @u32 $05, %01
	.loc 1 2 0
	mov DWORD PTR [rbp - 4], eax
	# $06 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	# ret $06
	leave
	ret
.size main, .-main

.global factorial
.type factorial, @function
factorial:
	enter 16, 0
	push rbx
	push r12
	# %01 @u32 = alloca
	# %02 @u32 = alloca
	# store @u32 n, %02
	mov DWORD PTR [rbp - 8], edi
	# $03 @u32 = load %02
	.loc 1 5 0
	mov ebx, DWORD PTR [rbp - 8]
	# $06 @bool = lth @u32 $03, 2
	cmp ebx, 2
	setb bl
	# if $06 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $09 @u32 = load %02
	.loc 1 8 0
	mov r12d, DWORD PTR [rbp - 8]
	# $10 @u32 = load %02
	mov ebx, DWORD PTR [rbp - 8]
	# $13 @u32 = sub @u32 $10, 1
	sub ebx, 1
	# arg @u32 $13
	# $15 @u32 = call factorial, 1
	mov edi, ebx
	call factorial
	# $16 @u32 = mul @u32 $09, $15
	.loc 1 8 0
	imul r12d, eax
	mov ebx, r12d
	# store @u32 $16, %01
	mov DWORD PTR [rbp - 4], ebx
	# goto L3
	jmp L3
L4:
	# store @u32 1, %01
	.loc 1 6 0
	mov DWORD PTR [rbp - 4], 1
	# goto L3
	jmp L3
L3:
	# $17 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	# ret $17
	pop r12
	pop rbx
	leave
	ret
.size factorial, .-factorial

.section .data
