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
	# %01 @u32 = alloca
	# $19 @u64 = call calling_convention(1, 2, 3, 4, 5, 6, 7, 8)
	.loc 1 3 0
	sub rsp, 16
	mov edi, 1
	mov esi, 2
	mov edx, 3
	mov ecx, 4
	mov r8d, 5
	mov r9d, 6
	mov DWORD PTR [rsp], 7
	mov DWORD PTR [rsp + 8], 8
	call calling_convention
	add rsp, 16
	# $20 @u32 = cast @u64 $19
	.loc 1 3 0
	mov ebx, eax
	# store @u32 $20, %01
	mov DWORD PTR [rbp - 4], ebx
	# $21 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	# ret $21
	leave
	ret
.size main, .-main

.global calling_convention
.type calling_convention, @function
calling_convention:
	# %01 @u64 = alloca
	# store @u64 1, %01
	.loc 1 6 0
	mov QWORD PTR [rsp - 8], 1
	# $12 @u64 = load %01
	mov rax, QWORD PTR [rsp - 8]
	# ret $12
	ret
.size calling_convention, .-calling_convention

.section .data
