	.file	"1.const.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%s(%d)\n"
	.text
.globl inc200
	.type	inc200, @function
inc200:
.LFB12:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ebx
	movl	%edi, %edx
	movl	$__FUNCTION__.2198, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leal	1(%rbx), %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE12:
	.size	inc200, .-inc200
.globl inc100
	.type	inc100, @function
inc100:
.LFB11:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ebx
	movl	%edi, %edx
	movl	$__FUNCTION__.2192, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leal	1(%rbx), %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11:
	.size	inc100, .-inc100
	.section	.rodata.str1.1
.LC1:
	.string	"%d %d\n"
	.text
.globl main
	.type	main, @function
main:
.LFB13:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$10, %edi
	call	inc100
	movl	%eax, %edx
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$10, %edi
	call	inc200
	movl	%eax, %ebx
	movl	$10, %edi
	call	inc200
	movl	%eax, %edx
	movl	%ebx, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE13:
	.size	main, .-main
	.section	.rodata
	.type	__FUNCTION__.2198, @object
	.size	__FUNCTION__.2198, 7
__FUNCTION__.2198:
	.string	"inc200"
	.type	__FUNCTION__.2192, @object
	.size	__FUNCTION__.2192, 7
__FUNCTION__.2192:
	.string	"inc100"
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-11)"
	.section	.note.GNU-stack,"",@progbits
