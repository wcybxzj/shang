	.file	"16.5sec_sig.c"
	.globl	loop
	.data
	.align 4
	.type	loop, @object
	.size	loop, 4
loop:
	.long	1
	.text
	.globl	alarm_handler
	.type	alarm_handler, @function
alarm_handler:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	$0, loop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	alarm_handler, .-alarm_handler
	.section	.rodata
.LC0:
	.string	"%lld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$20, %esp
	movl	$1, -16(%ebp)
	movl	$0, -12(%ebp)
	subl	$8, %esp
	pushl	$alarm_handler
	pushl	$14
	call	signal
	addl	$16, %esp
	subl	$12, %esp
	pushl	$5
	call	alarm
	addl	$16, %esp
	jmp	.L3
.L4:
	addl	$1, -16(%ebp)
	adcl	$0, -12(%ebp)
.L3:
	movl	loop, %eax
	testl	%eax, %eax
	jne	.L4
	subl	$4, %esp
	pushl	-12(%ebp)
	pushl	-16(%ebp)
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	subl	$12, %esp
	pushl	$0
	call	exit
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.9.2-10ubuntu13) 4.9.2"
	.section	.note.GNU-stack,"",@progbits
