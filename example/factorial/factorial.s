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
	mov r12d, eax
	# arg @u32 7
	# $09.0 @u32 = call factorial_while, 1
	.loc 1 2 0
	mov edi, 7
	call factorial_while
	mov ebx, eax
	# $10.0 @u32 = sub @u32 $05.0, $09.0
	.loc 1 2 0
	mov r10d, r12d
	sub r10d, ebx
	mov eax, r10d
	# ret $10.0
	pop r12
	pop rbx
	leave
	ret
.size main, .-main

.global factorial_recursive
.type factorial_recursive, @function
factorial_recursive:
	enter 0, 0
	push rbx
	push r12
	# $02.0 @u32 = $n.0
	mov r12d, edi
	# $06.0 @bool = equ @u32 $n.0, 1
	.loc 1 5 0
	cmp edi, 1
	sete bl
	# if $06.0 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $13.0 @u32 = sub @u32 $02.0, 1
	.loc 1 8 0
	mov r10d, r12d
	sub r10d, 1
	mov ebx, r10d
	# arg @u32 $13.0
	# $15.0 @u32 = call factorial_recursive, 1
	mov edi, ebx
	call factorial_recursive
	mov ebx, eax
	# $16.0 @u32 = mul @u32 $02.0, $15.0
	.loc 1 8 0
	mov r10d, r12d
	imul r10d, ebx
	mov ebx, r10d
	# $01.1 @u32 = $16.0
	# $01.0 @u32 = $01.1
	mov eax, ebx
	# goto L3
	jmp L3
L3:
	# ret $01.0
	pop r12
	pop rbx
	leave
	ret
L4:
	# $01.2 @u32 = 1
	.loc 1 6 0
	mov ebx, 1
	# $01.0 @u32 = $01.2
	mov eax, ebx
	# goto L3
	jmp L3
.size factorial_recursive, .-factorial_recursive

.global factorial_while
.type factorial_while, @function
factorial_while:
	push rbx
	push r12
	push r13
	# $02.0 @u32 = $n.0
	mov r12d, edi
	# $03.0 @u32 = 1
	.loc 1 11 0
	mov ebx, 1
	# $03.1 @u32 = $03.0
	mov eax, ebx
	# $02.1 @u32 = $02.0
	mov r13d, r12d
L9:
	# $09.0 @bool = neq @u32 $02.1, 0
	.loc 1 12 0
	cmp r13d, 0
	setne bl
	# if $09.0 then L10 else L11
	test bl, bl
	jnz L10
	jmp L11
L11:
	# ret $03.1
	pop r13
	pop r12
	pop rbx
	ret
L10:
	# $12.0 @u32 = mul @u32 $03.1, $02.1
	.loc 1 13 0
	mov r10d, eax
	imul r10d, r13d
	mov ebx, r10d
	# $03.2 @u32 = $12.0
	mov r12d, ebx
	# $16.0 @u32 = sub @u32 $02.1, 1
	.loc 1 14 0
	mov r10d, r13d
	sub r10d, 1
	mov ebx, r10d
	# $02.2 @u32 = $16.0
	# $03.1 @u32 = $03.2
	mov eax, r12d
	# $02.1 @u32 = $02.2
	mov r13d, ebx
	# goto L9
	jmp L9
.size factorial_while, .-factorial_while

.section .data
