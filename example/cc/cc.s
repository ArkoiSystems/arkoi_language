.intel_syntax noprefix
.file 1 "cc.ark"

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
	# arg @f32 0
	# arg @f32 2
	# arg @f32 3
	# arg @u32 4
	# arg @u32 5
	# arg @u32 6
	# arg @u32 7
	# arg @u32 8
	# $27.0 @u64 = call calling_convention, 8
	.loc 1 3 0
	mov edi, 4
	mov esi, 5
	mov edx, 6
	mov ecx, 7
	mov r8d, 8
	movss xmm0, DWORD PTR [float0]
	movss xmm1, DWORD PTR [float1]
	movss xmm2, DWORD PTR [float2]
	call calling_convention
	mov rbx, rax
	# $28.0 @u32 = cast @u64 $27.0
	.loc 1 3 0
	mov r10, rbx
	mov eax, r10d
	# ret $28.0
	pop rbx
	leave
	ret
.size main, .-main

.global calling_convention
.type calling_convention, @function
calling_convention:
	push rbx
	push r12
	push r13
	push r14
	# $03.0 @f32 = $b.0
	movss xmm9, xmm1
	# $04.0 @f32 = $c.0
	movss xmm8, xmm2
	# $10.0 @bool = 0
	.loc 1 6 0
	mov r12b, 0
	# $11.0 @bool = 0
	mov r13b, 0
	# $13.0 @bool = cast @f32 $a.0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm0
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# $11.1 @bool = $11.0
	mov r14b, r13b
	# if $13.0 then L8 else L9
	test bl, bl
	jnz L8
	jmp L9
L9:
	# if $11.1 then L4 else L5
	.loc 1 6 0
	test r14b, r14b
	jnz L4
	jmp L5
L5:
	# $18.0 @bool = cast @f32 $04.0
	.loc 1 6 0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# $10.1 @bool = $10.0
	# if $18.0 then L4 else L6
	test bl, bl
	jnz L4
	jmp L6
L6:
	# $20.0 @u64 = cast @bool $10.1
	.loc 1 6 0
	movzx r10, r12b
	mov rax, r10
	# ret $20.0
	pop r14
	pop r13
	pop r12
	pop rbx
	ret
L4:
	# $10.2 @bool = 1
	.loc 1 6 0
	mov bl, 1
	# $10.1 @bool = $10.2
	mov r12b, bl
	# goto L6
	jmp L6
L8:
	# $15.0 @bool = cast @f32 $03.0
	.loc 1 6 0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm9
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# $11.1 @bool = $11.0
	mov r14b, r13b
	# if $15.0 then L7 else L9
	test bl, bl
	jnz L7
	jmp L9
L7:
	# $11.2 @bool = 1
	.loc 1 6 0
	mov bl, 1
	# $11.1 @bool = $11.2
	mov r14b, bl
	# goto L9
	jmp L9
.size calling_convention, .-calling_convention

.section .data
	float0: .float	0.000000
	float1: .float	2.000000
	float2: .float	3.000000
