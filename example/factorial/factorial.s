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
	enter 0, 0
	push rbx
	push r12
	# arg @u32 7
	# $05.0 @u32 = call factorial_recursive, 1
	.loc 1 2 0
	mov edi, 7
	call factorial_recursive
	mov ebx, eax
	# arg @u32 7
	# $09.0 @u32 = call factorial_while, 1
	.loc 1 2 0
	mov edi, 7
	call factorial_while
	mov r12d, eax
	# $10.0 @u32 = sub @u32 $05.0, $09.0
	.loc 1 2 0
	sub ebx, r12d
	# $01.0 @u32 = $10.0
	# $11.0 @u32 = $01.0
	mov eax, ebx
	# ret $11.0
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
	# $02.0 @u32 = $n.0
	mov r12d, edi
	# $03.0 @u32 = $02.0
	.loc 1 5 0
	mov ebx, r12d
	# $06.0 @bool = equ @u32 $03.0, 1
	cmp ebx, 1
	sete bl
	# if $06.0 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $09.0 @u32 = $02.0
	.loc 1 8 0
	mov DWORD PTR [rbp - 4], r12d
	# $10.0 @u32 = $02.0
	mov ebx, r12d
	# $13.0 @u32 = sub @u32 $10.0, 1
	sub ebx, 1
	# arg @u32 $13.0
	# $15.0 @u32 = call factorial_recursive, 1
	mov edi, ebx
	call factorial_recursive
	mov ebx, eax
	# $16.0 @u32 = mul @u32 $09.0, $15.0
	.loc 1 8 0
	mov r10d, DWORD PTR [rbp - 4]
	imul r10d, ebx
	mov ebx, r10d
	# $01.1 @u32 = $16.0
	# $01.0 @u32 = $01.1
	# goto L3
	jmp L3
L3:
	# $17.0 @u32 = $01.0
	mov eax, ebx
	# ret $17.0
	pop r12
	pop rbx
	leave
	ret
L4:
	# $01.2 @u32 = 1
	.loc 1 6 0
	mov ebx, 1
	# $01.0 @u32 = $01.2
	# goto L3
	jmp L3
.size factorial_recursive, .-factorial_recursive

.global factorial_while
.type factorial_while, @function
factorial_while:
	push rbx
	push r12
	# $02.0 @u32 = $n.0
	mov ebx, edi
	# $03.0 @u32 = 1
	.loc 1 11 0
	mov r12d, 1
	# $03.1 @u32 = $03.0
	mov DWORD PTR [rsp - 4], r12d
	# $02.1 @u32 = $02.0
	mov DWORD PTR [rsp - 8], ebx
L9:
	# $06.0 @u32 = $02.1
	.loc 1 12 0
	mov ebx, DWORD PTR [rsp - 8]
	# $09.0 @bool = neq @u32 $06.0, 0
	cmp ebx, 0
	setne bl
	# if $09.0 then L10 else L11
	test bl, bl
	jnz L10
	jmp L11
L11:
	# $17.0 @u32 = $03.1
	.loc 1 15 0
	mov ebx, DWORD PTR [rsp - 4]
	# $01.0 @u32 = $17.0
	# $18.0 @u32 = $01.0
	mov eax, ebx
	# ret $18.0
	pop r12
	pop rbx
	ret
L10:
	# $10.0 @u32 = $03.1
	.loc 1 13 0
	mov r12d, DWORD PTR [rsp - 4]
	# $11.0 @u32 = $02.1
	mov ebx, DWORD PTR [rsp - 8]
	# $12.0 @u32 = mul @u32 $10.0, $11.0
	imul r12d, ebx
	mov ebx, r12d
	# $03.2 @u32 = $12.0
	mov r12d, ebx
	# $13.0 @u32 = $02.1
	.loc 1 14 0
	mov ebx, DWORD PTR [rsp - 8]
	# $16.0 @u32 = sub @u32 $13.0, 1
	sub ebx, 1
	# $02.2 @u32 = $16.0
	# $03.1 @u32 = $03.2
	mov DWORD PTR [rsp - 4], r12d
	# $02.1 @u32 = $02.2
	mov DWORD PTR [rsp - 8], ebx
	# goto L9
	jmp L9
.size factorial_while, .-factorial_while

.section .data
