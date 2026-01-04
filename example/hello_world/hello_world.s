.intel_syntax noprefix
.file 1 "hello_world.ark"

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
	# %01 @s32 = alloca
	# %01 @s32 = store 0
	.loc 1 2 0
	mov DWORD PTR [rsp - 4], 0
	# $03 @s32 = load %01
	mov eax, DWORD PTR [rsp - 4]
	# ret $03
	ret
.size main, .-main

.section .data
