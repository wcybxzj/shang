	.file	"17.5sec_sig_volatile.c"
	.data
	.align 4
	.type	loop, @object
	.size	loop, 4
loop:
	.long	1
	.text
.globl alarm_handler
	.type	alarm_handler, @function
alarm_handler:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	$0, loop(%rip)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	alarm_handler, .-alarm_handler
	.section	.rodata
.LC0:
	.string	"%lld\n"
	.text
.globl main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movq	$1, -8(%rbp)
	movl	$alarm_handler, %esi
	movl	$14, %edi
	call	signal
	movl	$5, %edi
	call	alarm
	jmp	.L4
.L5:
	addq	$1, -8(%rbp)
.L4:
	movl	loop(%rip), %eax
	testl	%eax, %eax
	jne	.L5
	movl	$.LC0, %eax
	movq	-8(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-11)"
	.section	.note.GNU-stack,"",@progbits
