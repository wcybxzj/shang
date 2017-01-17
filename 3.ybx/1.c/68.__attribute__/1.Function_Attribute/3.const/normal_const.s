	.file	"1.const.c"
	.section	.rodata
.LC0:
	.string	"%s(%d)\n"
	.text
.globl inc100
	.type	inc100, @function
inc100:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	$.LC0, %eax
	movl	-4(%rbp), %edx
	movl	$__FUNCTION__.2051, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	-4(%rbp), %eax
	addl	$1, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	inc100, .-inc100
.globl inc200
	.type	inc200, @function
inc200:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	$.LC0, %eax
	movl	-4(%rbp), %edx
	movl	$__FUNCTION__.2057, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	-4(%rbp), %eax
	addl	$1, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	inc200, .-inc200
	.section	.rodata
.LC1:
	.string	"%d %d\n"
	.text
.globl main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$10, %edi
	call	inc100
	movl	%eax, -8(%rbp)
	movl	$10, %edi
	call	inc100
	movl	%eax, -4(%rbp)
	movl	$.LC1, %eax
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$10, %edi
	call	inc200
	movl	%eax, -8(%rbp)
	movl	$10, %edi
	call	inc200
	movl	%eax, -4(%rbp)
	movl	$.LC1, %eax
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.section	.rodata
	.type	__FUNCTION__.2057, @object
	.size	__FUNCTION__.2057, 7
__FUNCTION__.2057:
	.string	"inc200"
	.type	__FUNCTION__.2051, @object
	.size	__FUNCTION__.2051, 7
__FUNCTION__.2051:
	.string	"inc100"
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-11)"
	.section	.note.GNU-stack,"",@progbits
