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
	# ret 0
	ret
.size main, .-main

.section .data
