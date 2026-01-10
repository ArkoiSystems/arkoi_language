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
	# $01.0 @u32 = $28.0
	# $29.0 @u32 = $01.0
	mov eax, ebx
	# ret $29.0
	pop rbx
	leave
	ret
.size main, .-main

.global calling_convention
.type calling_convention, @function
calling_convention:
	push rbx
	push r12
	# $02.0 @f32 = $a.0
	movss DWORD PTR [rsp - 4], xmm0
	# $03.0 @f32 = $b.0
	movss DWORD PTR [rsp - 8], xmm1
	# $04.0 @f32 = $c.0
	movss DWORD PTR [rsp - 12], xmm2
	# $10.0 @bool = 0
	.loc 1 6 0
	mov BYTE PTR [rsp - 13], 0
	# $11.0 @bool = 0
	mov BYTE PTR [rsp - 14], 0
	# $12.0 @f32 = $02.0
	movss xmm8, DWORD PTR [rsp - 4]
	# $13.0 @bool = cast @f32 $12.0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov BYTE PTR [rsp - 15], r10b
	# $11.1 @bool = $11.0
	mov r10b, BYTE PTR [rsp - 14]
	mov BYTE PTR [rsp - 16], r10b
	# if $13.0 then L8 else L9
	mov r10b, BYTE PTR [rsp - 15]
	test r10b, r10b
	jnz L8
	jmp L9
L9:
	# $16.0 @bool = $11.1
	.loc 1 6 0
	mov bl, BYTE PTR [rsp - 16]
	# if $16.0 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $17.0 @f32 = $04.0
	.loc 1 6 0
	movss xmm8, DWORD PTR [rsp - 12]
	# $18.0 @bool = cast @f32 $17.0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov r12b, r10b
	# $10.1 @bool = $10.0
	mov bl, BYTE PTR [rsp - 13]
	# if $18.0 then L4 else L6
	test r12b, r12b
	jnz L4
	jmp L6
L6:
	# $19.0 @bool = $10.1
	.loc 1 6 0
	# $20.0 @u64 = cast @bool $19.0
	movzx r10, bl
	mov rbx, r10
	# $01.0 @u64 = $20.0
	# $21.0 @u64 = $01.0
	mov rax, rbx
	# ret $21.0
	pop r12
	pop rbx
	ret
L4:
	# $10.2 @bool = 1
	.loc 1 6 0
	mov bl, 1
	# $10.1 @bool = $10.2
	# goto L6
	jmp L6
L8:
	# $14.0 @f32 = $03.0
	.loc 1 6 0
	movss xmm8, DWORD PTR [rsp - 8]
	# $15.0 @bool = cast @f32 $14.0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# $11.1 @bool = $11.0
	mov r10b, BYTE PTR [rsp - 14]
	mov BYTE PTR [rsp - 16], r10b
	# if $15.0 then L7 else L9
	test bl, bl
	jnz L7
	jmp L9
L7:
	# $11.2 @bool = 1
	.loc 1 6 0
	mov bl, 1
	# $11.1 @bool = $11.2
	mov BYTE PTR [rsp - 16], bl
	# goto L9
	jmp L9
.size calling_convention, .-calling_convention

.section .data
	float0: .float	0.000000
	float1: .float	2.000000
	float2: .float	3.000000
