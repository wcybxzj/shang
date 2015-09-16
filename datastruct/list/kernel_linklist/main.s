	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 10
	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$192, %rsp
	leaq	-72(%rbp), %rax
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	movq	%rcx, -8(%rbp)
	movl	$0, -52(%rbp)
	movq	%rax, -72(%rbp)
	movq	%rax, -64(%rbp)
	movq	$0, -80(%rbp)
	movq	$0, -88(%rbp)
	movq	$0, -96(%rbp)
	movq	l_main.str(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	l_main.str+8(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	l_main.str+16(%rip), %rax
	movq	%rax, -32(%rbp)
	movq	l_main.str+24(%rip), %rax
	movq	%rax, -24(%rbp)
	movl	$1, -100(%rbp)
LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$10, -100(%rbp)
	jg	LBB0_6
## BB#2:                                ##   in Loop: Header=BB0_1 Depth=1
	movl	$56, %eax
	movl	%eax, %edi
	callq	_malloc
	xorl	%ecx, %ecx
	movl	%ecx, %edi
	movq	%rax, -80(%rbp)
	cmpq	-80(%rbp), %rdi
	jne	LBB0_4
## BB#3:
	jmp	LBB0_6
LBB0_4:                                 ##   in Loop: Header=BB0_1 Depth=1
	callq	_rand
	movl	$32, %ecx
	movl	%ecx, %esi
	xorl	%edx, %edx
	leaq	L_.str(%rip), %r8
	movq	$-1, %rcx
	movl	$10, %edi
	movl	%edx, -172(%rbp)        ## 4-byte Spill
	cltd
	idivl	%edi
	addl	-100(%rbp), %edx
	movq	-80(%rbp), %r9
	movl	%edx, (%r9)
	movq	-80(%rbp), %r9
	addq	$4, %r9
	movl	-100(%rbp), %edx
	movq	%r9, %rdi
	movl	-172(%rbp), %r10d       ## 4-byte Reload
	movl	%edx, -176(%rbp)        ## 4-byte Spill
	movl	%r10d, %edx
	movl	-176(%rbp), %r9d        ## 4-byte Reload
	movb	$0, %al
	callq	___snprintf_chk
	leaq	-72(%rbp), %rsi
	movq	-80(%rbp), %rcx
	addq	$40, %rcx
	movq	%rcx, %rdi
	movl	%eax, -180(%rbp)        ## 4-byte Spill
	callq	_list_add_tail
## BB#5:                                ##   in Loop: Header=BB0_1 Depth=1
	movl	-100(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -100(%rbp)
	jmp	LBB0_1
LBB0_6:
	movq	-64(%rbp), %rax
	movq	%rax, -96(%rbp)
LBB0_7:                                 ## =>This Inner Loop Header: Depth=1
	leaq	-72(%rbp), %rax
	cmpq	%rax, -96(%rbp)
	je	LBB0_10
## BB#8:                                ##   in Loop: Header=BB0_7 Depth=1
	leaq	L_.str1(%rip), %rdi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rdx
	addq	$40, %rdx
	subq	%rdx, %rcx
	movq	-96(%rbp), %rdx
	movq	%rdx, -112(%rbp)
	movq	-112(%rbp), %rdx
	addq	%rcx, %rdx
	movq	%rdx, -120(%rbp)
	movq	-120(%rbp), %rcx
	movq	%rcx, -88(%rbp)
	movq	-88(%rbp), %rcx
	movl	(%rcx), %esi
	movq	-88(%rbp), %rcx
	addq	$4, %rcx
	movq	%rcx, %rdx
	movb	$0, %al
	callq	_printf
	movl	%eax, -184(%rbp)        ## 4-byte Spill
## BB#9:                                ##   in Loop: Header=BB0_7 Depth=1
	movq	-96(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -96(%rbp)
	jmp	LBB0_7
LBB0_10:
	leaq	L_.str2(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	-64(%rbp), %rdi
	movq	%rdi, -96(%rbp)
	movl	%eax, -188(%rbp)        ## 4-byte Spill
LBB0_11:                                ## =>This Inner Loop Header: Depth=1
	leaq	-72(%rbp), %rax
	cmpq	%rax, -96(%rbp)
	je	LBB0_16
## BB#12:                               ##   in Loop: Header=BB0_11 Depth=1
	leaq	-48(%rbp), %rsi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rdx
	addq	$40, %rdx
	subq	%rdx, %rcx
	movq	-96(%rbp), %rdx
	movq	%rdx, -128(%rbp)
	movq	-128(%rbp), %rdx
	addq	%rcx, %rdx
	movq	%rdx, -136(%rbp)
	movq	-136(%rbp), %rcx
	movq	%rcx, -88(%rbp)
	movq	-88(%rbp), %rcx
	addq	$4, %rcx
	movq	%rcx, %rdi
	callq	_strcmp
	cmpl	$0, %eax
	jne	LBB0_14
## BB#13:
	movq	-88(%rbp), %rax
	addq	$40, %rax
	movq	%rax, %rdi
	callq	_list_del
	jmp	LBB0_16
LBB0_14:                                ##   in Loop: Header=BB0_11 Depth=1
	jmp	LBB0_15
LBB0_15:                                ##   in Loop: Header=BB0_11 Depth=1
	movq	-96(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -96(%rbp)
	jmp	LBB0_11
LBB0_16:
	movq	-64(%rbp), %rax
	movq	%rax, -96(%rbp)
LBB0_17:                                ## =>This Inner Loop Header: Depth=1
	leaq	-72(%rbp), %rax
	cmpq	%rax, -96(%rbp)
	je	LBB0_20
## BB#18:                               ##   in Loop: Header=BB0_17 Depth=1
	leaq	L_.str1(%rip), %rdi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rdx
	addq	$40, %rdx
	subq	%rdx, %rcx
	movq	-96(%rbp), %rdx
	movq	%rdx, -144(%rbp)
	movq	-144(%rbp), %rdx
	addq	%rcx, %rdx
	movq	%rdx, -152(%rbp)
	movq	-152(%rbp), %rcx
	movq	%rcx, -88(%rbp)
	movq	-88(%rbp), %rcx
	movl	(%rcx), %esi
	movq	-88(%rbp), %rcx
	addq	$4, %rcx
	movq	%rcx, %rdx
	movb	$0, %al
	callq	_printf
	movl	%eax, -192(%rbp)        ## 4-byte Spill
## BB#19:                               ##   in Loop: Header=BB0_17 Depth=1
	movq	-96(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -96(%rbp)
	jmp	LBB0_17
LBB0_20:
	movq	-64(%rbp), %rax
	movq	%rax, -96(%rbp)
LBB0_21:                                ## =>This Inner Loop Header: Depth=1
	leaq	-72(%rbp), %rax
	cmpq	%rax, -96(%rbp)
	je	LBB0_24
## BB#22:                               ##   in Loop: Header=BB0_21 Depth=1
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rdx
	addq	$40, %rdx
	subq	%rdx, %rcx
	movq	-96(%rbp), %rdx
	movq	%rdx, -160(%rbp)
	movq	-160(%rbp), %rdx
	addq	%rcx, %rdx
	movq	%rdx, -168(%rbp)
	movq	-168(%rbp), %rcx
	movq	%rcx, -88(%rbp)
	movq	-88(%rbp), %rcx
	movq	%rcx, %rdi
	callq	_free
## BB#23:                               ##   in Loop: Header=BB0_21 Depth=1
	movq	-96(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -96(%rbp)
	jmp	LBB0_21
LBB0_24:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	LBB0_26
## BB#25:                               ## %SP_return
	xorl	%eax, %eax
	addq	$192, %rsp
	popq	%rbp
	retq
LBB0_26:                                ## %CallStackCheckFailBlk
	callq	___stack_chk_fail
	.cfi_endproc

	.align	4, 0x90
_list_add_tail:                         ## @list_add_tail
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp5:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	(%rsi), %rsi
	movq	-16(%rbp), %rdx
	callq	___list_add
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.align	4, 0x90
_list_del:                              ## @list_del
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp6:
	.cfi_def_cfa_offset 16
Ltmp7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp8:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %rdi
	movq	-8(%rbp), %rax
	movq	8(%rax), %rsi
	callq	___list_del
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.align	4, 0x90
___list_del:                            ## @__list_del
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp9:
	.cfi_def_cfa_offset 16
Ltmp10:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp11:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rsi
	movq	-16(%rbp), %rdi
	movq	%rsi, (%rdi)
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rdi
	movq	%rsi, 8(%rdi)
	popq	%rbp
	retq
	.cfi_endproc

	.align	4, 0x90
___list_add:                            ## @__list_add
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp12:
	.cfi_def_cfa_offset 16
Ltmp13:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp14:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rsi
	movq	%rdx, (%rsi)
	movq	-24(%rbp), %rdx
	movq	-8(%rbp), %rsi
	movq	%rdx, 8(%rsi)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rsi
	movq	%rdx, (%rsi)
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	%rdx, 8(%rsi)
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__const
	.align	4                       ## @main.str
l_main.str:
	.asciz	"shagua5\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"shagua%d"

L_.str1:                                ## @.str1
	.asciz	"%-3d%s\n"

L_.str2:                                ## @.str2
	.asciz	"*********delete********\n"


.subsections_via_symbols
