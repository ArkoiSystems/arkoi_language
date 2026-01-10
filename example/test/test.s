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
	enter 0, 0
	push rbx
	# arg @f64 5
	# $07.0 @bool = call ok, 1
	.loc 1 3 0
	movsd xmm0, QWORD PTR [float0]
	call ok
	mov bl, al
	# $08.0 @u32 = cast @bool $07.0
	.loc 1 3 0
	movzx r10d, bl
	mov ebx, r10d
	# $09.0 @u32 = mul @u32 1, $08.0
	mov r10d, 1
	imul r10d, ebx
	mov ebx, r10d
	# $12.0 @u32 = add @u32 $09.0, 1
	add ebx, 1
	# $13.0 @s32 = cast @u32 $12.0
	# arg @s32 $13.0
	# arg @f64 10.5
	# $18.0 @f32 = call test1, 2
	mov edi, ebx
	movsd xmm0, QWORD PTR [float1]
	call test1
	movss xmm8, xmm0
	# $20.0 @f32 = mul @f32 $18.0, 2.01
	.loc 1 3 0
	mulss xmm8, DWORD PTR [float2]
	# $23.0 @f32 = sub @f32 $20.0, 42
	subss xmm8, DWORD PTR [float3]
	# $24.0 @u64 = cast @f32 $23.0
	cvttss2si r10, xmm8
	mov rax, r10
	# ret $24.0
	pop rbx
	leave
	ret
.size main, .-main

.global ok
.type ok, @function
ok:
	enter 0, 0
	push rbx
	# $02.0 @f64 = $foo1.0
	movsd xmm8, xmm0
	# $06.0 @bool = gth @f64 $foo1.0, 5
	.loc 1 6 0
	ucomisd xmm0, QWORD PTR [float4]
	seta bl
	# if $06.0 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $12.0 @bool = goe @f64 $02.0, 10
	.loc 1 7 0
	ucomisd xmm8, QWORD PTR [float5]
	setae bl
	# if $12.0 then L7 else L8
	test bl, bl
	jnz L7
	jmp L8
L8:
	# $18.0 @bool = neq @f64 $02.0, 0
	.loc 1 8 0
	ucomisd xmm8, QWORD PTR [float6]
	setne bl
	# if $18.0 then L10 else L11
	test bl, bl
	jnz L10
	jmp L11
L11:
	# $02.4 @f64 = 21
	.loc 1 9 0
	movsd xmm8, QWORD PTR [float7]
	# $02.3 @f64 = $02.4
	# goto L12
	jmp L12
L12:
	# $02.2 @f64 = $02.3
	# goto L9
	jmp L9
L9:
	# $02.1 @f64 = $02.2
	# goto L6
	jmp L6
L6:
	# $25.0 @s32 = div @s32 4, 2
	.loc 1 10 0
	mov eax, 4
	mov r11d, 2
	idiv r11d
	mov ebx, eax
	# arg @s32 $25.0
	# arg @f64 $02.1
	# $29.0 @f32 = call test2, 2
	mov edi, ebx
	movsd xmm0, xmm8
	call test2
	movss xmm8, xmm0
	# $30.0 @bool = cast @f32 $29.0
	.loc 1 10 0
	xorps xmm11, xmm11
	ucomiss xmm11, xmm8
	setne r10b
	setp r11b
	or r10b, r11b
	mov al, r10b
	# ret $30.0
	pop rbx
	leave
	ret
L10:
	# $02.5 @f64 = 10
	.loc 1 8 0
	movsd xmm8, QWORD PTR [float8]
	# $02.3 @f64 = $02.5
	# goto L12
	jmp L12
L7:
	# $02.6 @f64 = 20
	.loc 1 7 0
	movsd xmm8, QWORD PTR [float9]
	# $02.2 @f64 = $02.6
	# goto L9
	jmp L9
L4:
	# $02.7 @f64 = 0
	.loc 1 6 0
	movsd xmm8, QWORD PTR [float10]
	# $02.1 @f64 = $02.7
	# goto L6
	jmp L6
.size ok, .-ok

.global test1
.type test1, @function
test1:
	push rbx
	# $02.0 @s32 = $foo2.0
	mov DWORD PTR [rsp - 4], edi
	# $03.0 @f64 = $bar.0
	movsd QWORD PTR [rsp - 12], xmm0
	# $07.0 @f64 = cast @s32 $foo2.0
	.loc 1 14 0
	cvtsi2sd xmm10, edi
	movsd xmm8, xmm10
	# $09.0 @bool = loe @f64 $07.0, $bar.0
	ucomisd xmm8, xmm0
	setbe bl
	# if $09.0 then L15 else L16
	test bl, bl
	jnz L15
	jmp L16
L16:
	# $21.0 @f64 = cast @s32 $02.0
	.loc 1 15 0
	cvtsi2sd xmm10, DWORD PTR [rsp - 4]
	movsd xmm8, xmm10
	# $23.0 @bool = equ @f64 $21.0, $03.0
	ucomisd xmm8, QWORD PTR [rsp - 12]
	sete bl
	# if $23.0 then L18 else L19
	test bl, bl
	jnz L18
	jmp L19
L19:
	# $27.0 @f64 = cast @s32 $02.0
	.loc 1 16 0
	cvtsi2sd xmm10, DWORD PTR [rsp - 4]
	movsd xmm8, xmm10
	# $29.0 @f64 = mul @f64 $27.0, $03.0
	mulsd xmm8, QWORD PTR [rsp - 12]
	# $30.0 @f32 = cast @f64 $29.0
	cvtsd2ss xmm8, xmm8
	# $04.3 @f32 = $30.0
	# $04.2 @f32 = $04.3
	# goto L20
	jmp L20
L20:
	# $04.1 @f32 = $04.2
	movss xmm0, xmm8
	# goto L17
	jmp L17
L17:
	# ret $04.1
	pop rbx
	ret
L18:
	# $25.0 @f32 = cast @s32 $02.0
	.loc 1 15 0
	cvtsi2ss xmm10, DWORD PTR [rsp - 4]
	movss xmm8, xmm10
	# $04.4 @f32 = $25.0
	# $04.2 @f32 = $04.4
	# goto L20
	jmp L20
L15:
	# $12.0 @f64 = cast @s32 $02.0
	.loc 1 14 0
	cvtsi2sd xmm10, DWORD PTR [rsp - 4]
	movsd xmm8, xmm10
	# $13.0 @f64 = mul @f64 $03.0, $12.0
	movsd xmm10, QWORD PTR [rsp - 12]
	mulsd xmm10, xmm8
	movsd xmm9, xmm10
	# $16.0 @bool = lth @s32 $02.0, $02.0
	mov r10d, DWORD PTR [rsp - 4]
	cmp r10d, DWORD PTR [rsp - 4]
	setl bl
	# $17.0 @f64 = cast @bool $16.0
	movzx r10d, bl
	cvtsi2sd xmm10, r10d
	movsd xmm8, xmm10
	# $18.0 @f64 = add @f64 $13.0, $17.0
	addsd xmm9, xmm8
	movsd xmm8, xmm9
	# $19.0 @f32 = cast @f64 $18.0
	cvtsd2ss xmm8, xmm8
	# $04.5 @f32 = $19.0
	# $04.1 @f32 = $04.5
	movss xmm0, xmm8
	# goto L17
	jmp L17
.size test1, .-test1

.global test2
.type test2, @function
test2:
	push rbx
	# $02.0 @s32 = $foo2.0
	mov DWORD PTR [rsp - 4], edi
	# $03.0 @f64 = $bar.0
	movsd QWORD PTR [rsp - 12], xmm0
	# $05.0 @f64 = cast @s32 $foo2.0
	.loc 1 20 0
	cvtsi2sd xmm10, edi
	movsd xmm8, xmm10
	# $07.0 @bool = lth @f64 $05.0, $bar.0
	ucomisd xmm8, xmm0
	setb bl
	# if $07.0 then L23 else L24
	test bl, bl
	jnz L23
	jmp L24
L24:
	# $14.0 @f64 = cast @s32 $02.0
	.loc 1 21 0
	cvtsi2sd xmm10, DWORD PTR [rsp - 4]
	movsd xmm8, xmm10
	# $16.0 @f64 = mul @f64 $14.0, $03.0
	mulsd xmm8, QWORD PTR [rsp - 12]
	# $03.2 @f64 = $16.0
	movsd xmm9, xmm8
	# $18.0 @f32 = cast @f64 $16.0
	.loc 1 22 0
	cvtsd2ss xmm8, xmm8
	# $01.1 @f32 = $18.0
	# goto L22
	jmp L22
L22:
	# ret $01.0
	pop rbx
	ret
L23:
	# $10.0 @f64 = cast @s32 $02.0
	.loc 1 20 0
	cvtsi2sd xmm10, DWORD PTR [rsp - 4]
	movsd xmm8, xmm10
	# $11.0 @f64 = mul @f64 $03.0, $10.0
	movsd xmm10, QWORD PTR [rsp - 12]
	mulsd xmm10, xmm8
	movsd xmm8, xmm10
	# $12.0 @f32 = cast @f64 $11.0
	cvtsd2ss xmm8, xmm8
	# $01.2 @f32 = $12.0
	# $03.1 @f64 = $03.0
	movsd xmm9, QWORD PTR [rsp - 12]
	# $01.0 @f32 = $01.2
	movss xmm0, xmm8
	# goto L22
	jmp L22
.size test2, .-test2

.section .data
	float0: .double	5.000000
	float1: .double	10.500000
	float2: .float	2.010000
	float3: .float	42.000000
	float4: .double	5.000000
	float5: .double	10.000000
	float6: .double	0.000000
	float7: .double	21.000000
	float8: .double	10.000000
	float9: .double	20.000000
	float10: .double	0.000000
