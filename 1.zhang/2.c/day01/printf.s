	.file	"printf.c"
	.section	.rodata
.LC0:
	.string	"\tthe \bnumber of a is %d\n"
.LC1:
	.string	"a = %d, \rb = %d, ret = %d\n"
	.text
.globl main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	movl	$100, -28(%rbp)
	movl	$2, -24(%rbp)
	addl	$1, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-28(%rbp), %ecx
	addl	$1, -28(%rbp)
	movl	$.LC0, %eax
	movl	-24(%rbp), %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	.cfi_offset 3, -24
	call	printf
	movl	%eax, -20(%rbp)
	movl	$.LC1, %eax
	movl	-20(%rbp), %ecx
	movl	-24(%rbp), %edx
	movl	-28(%rbp), %ebx
	movl	%ebx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	addq	$24, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-4)"
	.section	.note.GNU-stack,"",@progbits
