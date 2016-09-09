	.file	"17.5sec_sig_volatile.c"
	.text
.globl alarm_handler
	.type	alarm_handler, @function
alarm_handler:
.LFB21:
	.cfi_startproc
	movl	$0, loop(%rip)
	ret
	.cfi_endproc
.LFE21:
	.size	alarm_handler, .-alarm_handler
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%lld\n"
	.text
.globl main
	.type	main, @function
main:
.LFB22:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$alarm_handler, %esi
	movl	$14, %edi
	call	signal
	movl	$5, %edi
	call	alarm
	cmpl	$0, loop(%rip)
	jne	.L6
	movl	$1, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %edi
	call	exit
.L6:
	jmp	.L6
	.cfi_endproc
.LFE22:
	.size	main, .-main
	.data
	.align 4
	.type	loop, @object
	.size	loop, 4
loop:
	.long	1
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-11)"
	.section	.note.GNU-stack,"",@progbits
