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
	enter 16, 0
	push rbx
	# %01 @u32 = alloca
	# arg @f32 0
	# arg @f32 2
	# arg @f32 3
	# arg @u32 4
	# arg @u32 5
	# arg @u32 6
	# arg @u32 7
	# arg @u32 8
	# $27 @u64 = call calling_convention, 8
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
	# $28 @u32 = cast @u64 $27
	.loc 1 3 0
	# %01 @u32 = store $28
	mov DWORD PTR [rbp - 4], ebx
	# $29 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	# ret $29
	pop rbx
	leave
	ret
.size main, .-main

.global calling_convention
.type calling_convention, @function
calling_convention:
	push rbx
	# %01 @u64 = alloca
	# %02 @f32 = alloca
	# %03 @f32 = alloca
	# %04 @f32 = alloca
	# %02 @f32 = store a
	movss DWORD PTR [rsp - 12], xmm0
	# %03 @f32 = store b
	movss DWORD PTR [rsp - 16], xmm1
	# %04 @f32 = store c
	movss DWORD PTR [rsp - 20], xmm2
	# %10 @bool = alloca
	# %10 @bool = store 0
	.loc 1 6 0
	mov BYTE PTR [rsp - 21], 0
	# %11 @bool = alloca
	# %11 @bool = store 0
	mov BYTE PTR [rsp - 22], 0
	# $12 @f32 = load %02
	movss xmm8, DWORD PTR [rsp - 12]
	# $13 @bool = cast @f32 $12
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# if $13 then L8 else L9
	test bl, bl
	jnz L8
	jmp L9
L9:
	# $11 @bool = phi [  ]
	# $16 @bool = load %11
	.loc 1 6 0
	mov bl, BYTE PTR [rsp - 22]
	# if $16 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $17 @f32 = load %04
	.loc 1 6 0
	movss xmm8, DWORD PTR [rsp - 20]
	# $18 @bool = cast @f32 $17
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# if $18 then L4 else L6
	test bl, bl
	jnz L4
	jmp L6
L6:
	# $10 @bool = phi [  ]
	# $19 @bool = load %10
	.loc 1 6 0
	mov bl, BYTE PTR [rsp - 21]
	# $20 @u64 = cast @bool $19
	movzx r10, bl
	mov rbx, r10
	# %01 @u64 = store $20
	mov QWORD PTR [rsp - 8], rbx
	# $21 @u64 = load %01
	mov rax, QWORD PTR [rsp - 8]
	# ret $21
	pop rbx
	ret
L4:
	# %10 @bool = store 1
	.loc 1 6 0
	mov BYTE PTR [rsp - 21], 1
	# goto L6
	jmp L6
L8:
	# $14 @f32 = load %03
	.loc 1 6 0
	movss xmm8, DWORD PTR [rsp - 16]
	# $15 @bool = cast @f32 $14
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# if $15 then L7 else L9
	test bl, bl
	jnz L7
	jmp L9
L7:
	# %11 @bool = store 1
	.loc 1 6 0
	mov BYTE PTR [rsp - 22], 1
	# goto L9
	jmp L9
.size calling_convention, .-calling_convention

.section .data
	float0: .float	0.000000
	float1: .float	2.000000
	float2: .float	3.000000
