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
	# $10.0 @u32 = sub @u32 $05.0, $09.0
	.loc 1 2 0
	mov r10d, ebx
	sub r10d, eax
	mov eax, r10d
	# ret $10.0
	pop rbx
	leave
	ret
.size main, .-main

.global factorial_recursive
.type factorial_recursive, @function
factorial_recursive:
	enter 0, 0
	push rbx
	# $02.0 @u32 = $n.0
	mov ebx, edi
	# $06.0 @bool = equ @u32 $n.0, 1
	.loc 1 5 0
	cmp edi, 1
	sete al
	# if $06.0 then L4 else L5
	test al, al
	jnz L4
	jmp L5
L5:
	# $13.0 @u32 = sub @u32 $02.0, 1
	.loc 1 8 0
	mov r10d, ebx
	sub r10d, 1
	mov eax, r10d
	# arg @u32 $13.0
	# $15.0 @u32 = call factorial_recursive, 1
	mov edi, eax
	call factorial_recursive
	# $16.0 @u32 = mul @u32 $02.0, $15.0
	.loc 1 8 0
	mov r10d, ebx
	imul r10d, eax
	mov eax, r10d
	# $01.1 @u32 = $16.0
	# $01.0 @u32 = $01.1
	# goto L3
	jmp L3
L3:
	# ret $01.0
	pop rbx
	leave
	ret
L4:
	# $01.2 @u32 = 1
	.loc 1 6 0
	mov eax, 1
	# $01.0 @u32 = $01.2
	# goto L3
	jmp L3
.size factorial_recursive, .-factorial_recursive

.global factorial_while
.type factorial_while, @function
factorial_while:
	# $02.0 @u32 = $n.0
	mov ecx, edi
	# $03.0 @u32 = 1
	.loc 1 11 0
	mov eax, 1
	# $03.1 @u32 = $03.0
	# $02.1 @u32 = $02.0
L9:
	# $09.0 @bool = neq @u32 $02.1, 0
	.loc 1 12 0
	cmp ecx, 0
	setne dl
	# if $09.0 then L10 else L11
	test dl, dl
	jnz L10
	jmp L11
L11:
	# ret $03.1
	ret
L10:
	# $12.0 @u32 = mul @u32 $03.1, $02.1
	.loc 1 13 0
	imul eax, ecx
	# $03.2 @u32 = $12.0
	# $16.0 @u32 = sub @u32 $02.1, 1
	.loc 1 14 0
	sub ecx, 1
	# $02.2 @u32 = $16.0
	# $03.1 @u32 = $03.2
	# $02.1 @u32 = $02.2
	# goto L9
	jmp L9
.size factorial_while, .-factorial_while

.section .data
