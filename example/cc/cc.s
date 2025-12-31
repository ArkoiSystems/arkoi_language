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
	# arg @u32 1
	# arg @u32 2
	# arg @u32 3
	# arg @u32 4
	# arg @u32 5
	# arg @u32 6
	# arg @u32 7
	# arg @u32 8
	# $27 @u64 = call calling_convention, 8
	.loc 1 3 0
	mov edi, 1
	mov esi, 2
	mov edx, 3
	mov ecx, 4
	mov r8d, 5
	mov r9d, 6
	push 8
	push 7
	call calling_convention
	add rsp, 16
	# $28 @u32 = cast @u64 $27
	.loc 1 3 0
	mov ebx, eax
	# store @u32 $28, %01
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
