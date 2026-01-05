.intel_syntax noprefix
.file 1 "test.ark"

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
	# %01 @u64 = alloca
	# arg @f64 5
	# $07 @bool = call ok, 1
	.loc 1 3 0
	movsd xmm0, QWORD PTR [float0]
	call ok
	mov bl, al
	# $08 @u32 = cast @bool $07
	.loc 1 3 0
	movzx r10d, bl
	mov ebx, r10d
	# $09 @u32 = mul @u32 1, $08
	mov r10d, 1
	imul r10d, ebx
	mov ebx, r10d
	# $12 @u32 = add @u32 $09, 1
	add ebx, 1
	# $13 @s32 = cast @u32 $12
	# arg @s32 $13
	# arg @f64 10.5
	# $18 @f32 = call test1, 2
	mov edi, ebx
	movsd xmm0, QWORD PTR [float1]
	call test1
	movss xmm8, xmm0
	# $20 @f32 = mul @f32 $18, 2.01
	.loc 1 3 0
	mulss xmm8, DWORD PTR [float2]
	# $23 @f32 = sub @f32 $20, 42
	subss xmm8, DWORD PTR [float3]
	# $24 @u64 = cast @f32 $23
	cvttss2si r10, xmm8
	mov rbx, r10
	# %01 @u64 = store $24
	mov QWORD PTR [rbp - 8], rbx
	# $25 @u64 = load %01
	mov rax, QWORD PTR [rbp - 8]
	# ret $25
	pop rbx
	leave
	ret
.size main, .-main

.global ok
.type ok, @function
ok:
	enter 16, 0
	push rbx
	# %01 @bool = alloca
	# %02 @f64 = alloca
	# %02 @f64 = store foo1
	movsd QWORD PTR [rbp - 9], xmm0
	# $03 @f64 = load %02
	.loc 1 6 0
	movsd xmm8, QWORD PTR [rbp - 9]
	# $06 @bool = gth @f64 $03, 5
	ucomisd xmm8, QWORD PTR [float4]
	seta bl
	# if $06 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L4:
	# %02 @f64 = store 0
	.loc 1 6 0
	movsd xmm10, QWORD PTR [float5]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L5:
	# $09 @f64 = load %02
	.loc 1 7 0
	movsd xmm8, QWORD PTR [rbp - 9]
	# $12 @bool = goe @f64 $09, 10
	ucomisd xmm8, QWORD PTR [float6]
	setae bl
	# if $12 then L7 else L8
	test bl, bl
	jnz L7
	jmp L8
L7:
	# %02 @f64 = store 20
	.loc 1 7 0
	movsd xmm10, QWORD PTR [float7]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L8:
	# $15 @f64 = load %02
	.loc 1 8 0
	movsd xmm8, QWORD PTR [rbp - 9]
	# $18 @bool = neq @f64 $15, 0
	ucomisd xmm8, QWORD PTR [float8]
	setne bl
	# if $18 then L10 else L11
	test bl, bl
	jnz L10
	jmp L11
L10:
	# %02 @f64 = store 10
	.loc 1 8 0
	movsd xmm10, QWORD PTR [float9]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L11:
	# %02 @f64 = store 21
	.loc 1 9 0
	movsd xmm10, QWORD PTR [float10]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L6:
	# $25 @s32 = div @s32 4, 2
	.loc 1 10 0
	mov eax, 4
	mov r11d, 2
	idiv r11d
	mov ebx, eax
	# arg @s32 $25
	# $27 @f64 = load %02
	movsd xmm8, QWORD PTR [rbp - 9]
	# arg @f64 $27
	# $29 @f32 = call test2, 2
	mov edi, ebx
	movsd xmm0, xmm8
	call test2
	movss xmm8, xmm0
	# $30 @bool = cast @f32 $29
	.loc 1 10 0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# %01 @bool = store $30
	mov BYTE PTR [rbp - 1], bl
	# $31 @bool = load %01
	mov al, BYTE PTR [rbp - 1]
	# ret $31
	pop rbx
	leave
	ret
.size ok, .-ok

.global test1
.type test1, @function
test1:
	push rbx
	push r12
	# %01 @f32 = alloca
	# %02 @s32 = alloca
	# %03 @f64 = alloca
	# %02 @s32 = store foo2
	mov DWORD PTR [rsp - 8], edi
	# %03 @f64 = store bar
	movsd QWORD PTR [rsp - 16], xmm0
	# %04 @f32 = alloca
	# %04 @f32 = store 0
	.loc 1 13 0
	movss xmm10, DWORD PTR [float11]
	movss DWORD PTR [rsp - 20], xmm10
	# $06 @s32 = load %02
	.loc 1 14 0
	mov ebx, DWORD PTR [rsp - 8]
	# $07 @f64 = cast @s32 $06
	cvtsi2sd xmm10, ebx
	movsd xmm8, xmm10
	# $08 @f64 = load %03
	movsd xmm9, QWORD PTR [rsp - 16]
	# $09 @bool = loe @f64 $07, $08
	ucomisd xmm8, xmm9
	setbe bl
	# if $09 then L15 else L16
	test bl, bl
	jnz L15
	jmp L16
L15:
	# $10 @f64 = load %03
	.loc 1 14 0
	movsd xmm8, QWORD PTR [rsp - 16]
	# $11 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $12 @f64 = cast @s32 $11
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $13 @f64 = mul @f64 $10, $12
	mulsd xmm8, xmm9
	movsd xmm9, xmm8
	# $14 @s32 = load %02
	mov r12d, DWORD PTR [rsp - 8]
	# $15 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $16 @bool = lth @s32 $14, $15
	cmp r12d, ebx
	setl bl
	# $17 @f64 = cast @bool $16
	movzx r10d, bl
	cvtsi2sd xmm10, r10d
	movsd xmm8, xmm10
	# $18 @f64 = add @f64 $13, $17
	addsd xmm9, xmm8
	movsd xmm8, xmm9
	# $19 @f32 = cast @f64 $18
	cvtsd2ss xmm8, xmm8
	# %04 @f32 = store $19
	movss DWORD PTR [rsp - 20], xmm8
	# goto L17
	jmp L17
L16:
	# $20 @s32 = load %02
	.loc 1 15 0
	mov ebx, DWORD PTR [rsp - 8]
	# $21 @f64 = cast @s32 $20
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $22 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $23 @bool = equ @f64 $21, $22
	ucomisd xmm9, xmm8
	sete bl
	# if $23 then L18 else L19
	test bl, bl
	jnz L18
	jmp L19
L18:
	# $24 @s32 = load %02
	.loc 1 15 0
	mov ebx, DWORD PTR [rsp - 8]
	# $25 @f32 = cast @s32 $24
	cvtsi2ss xmm10, ebx
	movss xmm8, xmm10
	# %04 @f32 = store $25
	movss DWORD PTR [rsp - 20], xmm8
	# goto L17
	jmp L17
L19:
	# $26 @s32 = load %02
	.loc 1 16 0
	mov ebx, DWORD PTR [rsp - 8]
	# $27 @f64 = cast @s32 $26
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $28 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $29 @f64 = mul @f64 $27, $28
	mulsd xmm9, xmm8
	movsd xmm8, xmm9
	# $30 @f32 = cast @f64 $29
	cvtsd2ss xmm8, xmm8
	# %04 @f32 = store $30
	movss DWORD PTR [rsp - 20], xmm8
	# goto L17
	jmp L17
L17:
	# $31 @f32 = load %04
	.loc 1 17 0
	movss xmm8, DWORD PTR [rsp - 20]
	# %01 @f32 = store $31
	movss DWORD PTR [rsp - 4], xmm8
	# $32 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $32
	pop r12
	pop rbx
	ret
.size test1, .-test1

.global test2
.type test2, @function
test2:
	push rbx
	# %01 @f32 = alloca
	# %02 @s32 = alloca
	# %03 @f64 = alloca
	# %02 @s32 = store foo2
	mov DWORD PTR [rsp - 8], edi
	# %03 @f64 = store bar
	movsd QWORD PTR [rsp - 16], xmm0
	# $04 @s32 = load %02
	.loc 1 20 0
	mov ebx, DWORD PTR [rsp - 8]
	# $05 @f64 = cast @s32 $04
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $06 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $07 @bool = lth @f64 $05, $06
	ucomisd xmm9, xmm8
	setb bl
	# if $07 then L23 else L24
	test bl, bl
	jnz L23
	jmp L24
L23:
	# $08 @f64 = load %03
	.loc 1 20 0
	movsd xmm8, QWORD PTR [rsp - 16]
	# $09 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $10 @f64 = cast @s32 $09
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $11 @f64 = mul @f64 $08, $10
	mulsd xmm8, xmm9
	# $12 @f32 = cast @f64 $11
	cvtsd2ss xmm8, xmm8
	# %01 @f32 = store $12
	movss DWORD PTR [rsp - 4], xmm8
	# goto L22
	jmp L22
L24:
	# $13 @s32 = load %02
	.loc 1 21 0
	mov ebx, DWORD PTR [rsp - 8]
	# $14 @f64 = cast @s32 $13
	cvtsi2sd xmm10, ebx
	movsd xmm8, xmm10
	# $15 @f64 = load %03
	movsd xmm9, QWORD PTR [rsp - 16]
	# $16 @f64 = mul @f64 $14, $15
	mulsd xmm8, xmm9
	# %03 @f64 = store $16
	movsd QWORD PTR [rsp - 16], xmm8
	# $17 @f64 = load %03
	.loc 1 22 0
	movsd xmm8, QWORD PTR [rsp - 16]
	# $18 @f32 = cast @f64 $17
	cvtsd2ss xmm8, xmm8
	# %01 @f32 = store $18
	movss DWORD PTR [rsp - 4], xmm8
	# goto L22
	jmp L22
L22:
	# $19 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $19
	pop rbx
	ret
.size test2, .-test2

.section .data
	float0: .double	5.000000
	float1: .double	10.500000
	float2: .float	2.010000
	float3: .float	42.000000
	float4: .double	5.000000
	float5: .double	0.000000
	float6: .double	10.000000
	float7: .double	20.000000
	float8: .double	0.000000
	float9: .double	10.000000
	float10: .double	21.000000
	float11: .float	0.000000
