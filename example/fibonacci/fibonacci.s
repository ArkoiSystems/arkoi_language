.intel_syntax noprefix
.file 1 "fibonacci.ark"

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
	# arg @u32 20
	# $05.0 @u32 = call fib, 1
	.loc 1 2 0
	mov edi, 20
	call fib
	# ret $05.0
	leave
	ret
.size main, .-main

.global fib
.type fib, @function
fib:
	enter 0, 0
	push rbx
	push r12
	# $02.0 @u32 = $n.0
	mov r12d, edi
	# $06.0 @bool = loe @u32 $n.0, 1
	.loc 1 6 0
	cmp edi, 1
	setbe cl
	# if $06.0 then L4 else L5
	test cl, cl
	jnz L4
	jmp L5
L5:
	# $11.0 @u32 = sub @u32 $02.0, 1
	.loc 1 8 0
	mov r10d, r12d
	sub r10d, 1
	mov ecx, r10d
	# arg @u32 $11.0
	# $13.0 @u32 = call fib, 1
	mov edi, ecx
	call fib
	mov ebx, eax
	# $17.0 @u32 = sub @u32 $02.0, 2
	.loc 1 8 0
	mov r10d, r12d
	sub r10d, 2
	mov ecx, r10d
	# arg @u32 $17.0
	# $19.0 @u32 = call fib, 1
	mov edi, ecx
	call fib
	mov ecx, eax
	# $20.0 @u32 = add @u32 $13.0, $19.0
	.loc 1 8 0
	mov r10d, ebx
	add r10d, ecx
	mov ecx, r10d
	# $01.1 @u32 = $20.0
	# goto L3
	jmp L3
L3:
	# ret $01.0
	pop r12
	pop rbx
	leave
	ret
L4:
	# $01.2 @u32 = $02.0
	.loc 1 7 0
	mov eax, r12d
	# $01.0 @u32 = $01.2
	# goto L3
	jmp L3
.size fib, .-fib

.section .data
