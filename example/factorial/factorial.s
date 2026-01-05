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
	push rbx
	push r12
	# %01 @u32 = alloca
	# arg @u32 7
	# $05 @u32 = call factorial_recursive, 1
	.loc 1 2 0
	mov edi, 7
	call factorial_recursive
	mov ebx, eax
	# arg @u32 7
	# $09 @u32 = call factorial_while, 1
	.loc 1 2 0
	mov edi, 7
	call factorial_while
	mov r12d, eax
	# $10 @u32 = sub @u32 $05, $09
	.loc 1 2 0
	sub ebx, r12d
	# %01 @u32 = store $10
	mov DWORD PTR [rbp - 4], ebx
	# $11 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	# ret $11
	pop r12
	pop rbx
	leave
	ret
.size main, .-main

.global factorial_recursive
.type factorial_recursive, @function
factorial_recursive:
	enter 16, 0
	push rbx
	push r12
	# %01 @u32 = alloca
	# %02 @u32 = alloca
	# %02 @u32 = store n
	mov DWORD PTR [rbp - 8], edi
	# $03 @u32 = load %02
	.loc 1 5 0
	mov ebx, DWORD PTR [rbp - 8]
	# $06 @bool = equ @u32 $03, 1
	cmp ebx, 1
	sete bl
	# if $06 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L4:
	# %01 @u32 = store 1
	.loc 1 6 0
	mov DWORD PTR [rbp - 4], 1
	# goto L3
	jmp L3
L5:
	# $09 @u32 = load %02
	.loc 1 8 0
	mov r12d, DWORD PTR [rbp - 8]
	# $10 @u32 = load %02
	mov ebx, DWORD PTR [rbp - 8]
	# $13 @u32 = sub @u32 $10, 1
	sub ebx, 1
	# arg @u32 $13
	# $15 @u32 = call factorial_recursive, 1
	mov edi, ebx
	call factorial_recursive
	mov ebx, eax
	# $16 @u32 = mul @u32 $09, $15
	.loc 1 8 0
	imul r12d, ebx
	mov ebx, r12d
	# %01 @u32 = store $16
	mov DWORD PTR [rbp - 4], ebx
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
.size factorial_recursive, .-factorial_recursive

.global factorial_while
.type factorial_while, @function
factorial_while:
	push rbx
	push r12
	# %01 @u32 = alloca
	# %02 @u32 = alloca
	# %02 @u32 = store n
	mov DWORD PTR [rsp - 8], edi
	# %03 @u32 = alloca
	# %03 @u32 = store 1
	.loc 1 11 0
	mov DWORD PTR [rsp - 12], 1
L9:
	# $06 @u32 = load %02
	.loc 1 12 0
	mov ebx, DWORD PTR [rsp - 8]
	# $09 @bool = neq @u32 $06, 0
	cmp ebx, 0
	setne bl
	# if $09 then L10 else L11
	test bl, bl
	jnz L10
	jmp L11
L10:
	# $10 @u32 = load %03
	.loc 1 13 0
	mov r12d, DWORD PTR [rsp - 12]
	# $11 @u32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $12 @u32 = mul @u32 $10, $11
	imul r12d, ebx
	mov ebx, r12d
	# %03 @u32 = store $12
	mov DWORD PTR [rsp - 12], ebx
	# $13 @u32 = load %02
	.loc 1 14 0
	mov ebx, DWORD PTR [rsp - 8]
	# $16 @u32 = sub @u32 $13, 1
	sub ebx, 1
	# %02 @u32 = store $16
	mov DWORD PTR [rsp - 8], ebx
	# goto L9
	jmp L9
L11:
	# $17 @u32 = load %03
	.loc 1 15 0
	mov ebx, DWORD PTR [rsp - 12]
	# %01 @u32 = store $17
	mov DWORD PTR [rsp - 4], ebx
	# $18 @u32 = load %01
	mov eax, DWORD PTR [rsp - 4]
	# ret $18
	pop r12
	pop rbx
	ret
.size factorial_while, .-factorial_while

.section .data
