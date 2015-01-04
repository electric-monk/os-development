	.file	"mem_physical.cpp"
	.text
.Ltext0:
	.globl	_ZN15CPhysicalMemory7MinimumE
	.section	.rodata
	.align 4
	.type	_ZN15CPhysicalMemory7MinimumE, @object
	.size	_ZN15CPhysicalMemory7MinimumE, 4
_ZN15CPhysicalMemory7MinimumE:
	.zero	4
	.globl	_ZN15CPhysicalMemory7MaximumE
	.align 4
	.type	_ZN15CPhysicalMemory7MaximumE, @object
	.size	_ZN15CPhysicalMemory7MaximumE, 4
_ZN15CPhysicalMemory7MaximumE:
	.long	-1
	.globl	_ZN15CPhysicalMemory7InvalidE
	.align 4
	.type	_ZN15CPhysicalMemory7InvalidE, @object
	.size	_ZN15CPhysicalMemory7InvalidE, 4
_ZN15CPhysicalMemory7InvalidE:
	.long	-1
	.local	_ZL8basePage
	.comm	_ZL8basePage,4096,32
	.local	_ZL10latestPage
	.comm	_ZL10latestPage,4,4
	.local	_ZL9usedPages
	.comm	_ZL9usedPages,4,4
	.text
	.align 2
	.globl	_ZN15CPhysicalMemory4InitEv
	.type	_ZN15CPhysicalMemory4InitEv, @function
_ZN15CPhysicalMemory4InitEv:
.LFB0:
	.file 1 "mem_physical.cpp"
	.loc 1 31 0
	.cfi_startproc
	pushl	%ebp
.LCFI0:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI1:
	.cfi_def_cfa_register 5
	.loc 1 32 0
	movl	$0, _ZL8basePage
	.loc 1 33 0
	movl	$0, _ZL8basePage+4
	.loc 1 34 0
	movl	$0, _ZL8basePage+8
	.loc 1 35 0
	movl	$_ZL8basePage, _ZL10latestPage
	.loc 1 36 0
	movl	$0, _ZL9usedPages
	.loc 1 37 0
	popl	%ebp
.LCFI2:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	_ZN15CPhysicalMemory4InitEv, .-_ZN15CPhysicalMemory4InitEv
	.type	_ZL10GetNewPagev, @function
_ZL10GetNewPagev:
.LFB1:
	.loc 1 40 0
	.cfi_startproc
	pushl	%ebp
.LCFI3:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI4:
	.cfi_def_cfa_register 5
	subl	$56, %esp
.LBB2:
	.loc 1 41 0
	movl	$-1, 8(%esp)
	movl	$0, 4(%esp)
	movl	$1, (%esp)
	call	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	movl	%eax, -12(%ebp)
	.loc 1 42 0
	movl	currentAddressSpace, %eax
	movl	$1, 16(%esp)
	movl	-12(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	movl	%eax, -16(%ebp)
	.loc 1 43 0
	movl	_ZL10latestPage, %edx
	movl	-16(%ebp), %eax
	movl	%edx, (%eax)
	.loc 1 44 0
	movl	-16(%ebp), %eax
	movl	$0, 4(%eax)
	.loc 1 45 0
	movl	-16(%ebp), %eax
	movl	$0, 8(%eax)
	.loc 1 46 0
	movl	_ZL10latestPage, %eax
	movl	-16(%ebp), %edx
	movl	%edx, 4(%eax)
	.loc 1 47 0
	movl	-16(%ebp), %eax
	movl	%eax, _ZL10latestPage
.LBE2:
	.loc 1 48 0
	leave
.LCFI5:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_ZL10GetNewPagev, .-_ZL10GetNewPagev
	.type	_ZL8TidyPageP10PAGE_STACK, @function
_ZL8TidyPageP10PAGE_STACK:
.LFB2:
	.loc 1 51 0
	.cfi_startproc
	pushl	%ebp
.LCFI6:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI7:
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$36, %esp
	.cfi_offset 3, -12
.LBB3:
.LBB4:
	.loc 1 52 0
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	testl	%eax, %eax
	je	.L4
.LBB5:
	.loc 1 57 0
	movl	$0, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	.L5
.L7:
	.loc 1 59 0
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	testl	%eax, %eax
	je	.L6
	.loc 1 60 0
	movl	-16(%ebp), %eax
	movl	8(%ebp), %ecx
	leal	2(%eax), %ebx
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$2, %edx
	leal	(%eax,%edx,8), %edx
	movl	(%edx), %eax
	movl	4(%edx), %edx
	movl	%eax, (%ecx,%ebx,8)
	movl	%edx, 4(%ecx,%ebx,8)
	addl	$1, -16(%ebp)
.L6:
	.loc 1 57 0
	addl	$1, -12(%ebp)
.L5:
	.loc 1 57 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %edx
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	cmpl	%eax, %edx
	setb	%al
	testb	%al, %al
	jne	.L7
	.loc 1 62 0 is_stmt 1
	movl	-16(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, 8(%eax)
	.loc 1 65 0
	cmpl	$0, -16(%ebp)
	jne	.L13
.L4:
.LBE5:
.LBE4:
	.loc 1 70 0
	cmpl	$_ZL8basePage, 8(%ebp)
	je	.L14
.L9:
	.loc 1 74 0
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	testl	%eax, %eax
	je	.L10
	.loc 1 75 0
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	8(%ebp), %edx
	movl	(%edx), %edx
	movl	%edx, (%eax)
.L10:
	.loc 1 76 0
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	je	.L11
	.loc 1 77 0
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	8(%ebp), %edx
	movl	4(%edx), %edx
	movl	%edx, 4(%eax)
.L11:
	.loc 1 78 0
	movl	_ZL10latestPage, %eax
	cmpl	8(%ebp), %eax
	jne	.L12
	.loc 1 79 0
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, _ZL10latestPage
.L12:
	.loc 1 82 0
	movl	currentAddressSpace, %eax
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo7AddressEPv
	movl	%eax, -20(%ebp)
	.loc 1 83 0
	movl	currentAddressSpace, %eax
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo5UnmapEPv
	.loc 1 84 0
	movl	$1, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory12ReleasePagesEPvm
	jmp	.L3
.L13:
.LBB7:
.LBB6:
	.loc 1 66 0
	nop
	jmp	.L3
.L14:
.LBE6:
.LBE7:
	.loc 1 71 0
	nop
.L3:
.LBE3:
	.loc 1 85 0
	addl	$36, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI8:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	_ZL8TidyPageP10PAGE_STACK, .-_ZL8TidyPageP10PAGE_STACK
	.type	_ZL9DropPagesv, @function
_ZL9DropPagesv:
.LFB3:
	.loc 1 88 0
	.cfi_startproc
	pushl	%ebp
.LCFI9:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI10:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB8:
	.loc 1 92 0
	movl	$_ZL8basePage, -12(%ebp)
	jmp	.L16
.L17:
	.loc 1 94 0 discriminator 2
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -16(%ebp)
	.loc 1 95 0 discriminator 2
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZL8TidyPageP10PAGE_STACK
	.loc 1 92 0 discriminator 2
	movl	-16(%ebp), %eax
	movl	%eax, -12(%ebp)
.L16:
	.loc 1 92 0 is_stmt 0 discriminator 1
	cmpl	$0, -12(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L17
.LBE8:
	.loc 1 97 0 is_stmt 1
	leave
.LCFI11:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	_ZL9DropPagesv, .-_ZL9DropPagesv
	.align 2
	.globl	_ZN15CPhysicalMemory8AddChunkEPvm
	.type	_ZN15CPhysicalMemory8AddChunkEPvm, @function
_ZN15CPhysicalMemory8AddChunkEPvm:
.LFB4:
	.loc 1 100 0
	.cfi_startproc
	pushl	%ebp
.LCFI12:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI13:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB9:
	.loc 1 106 0
	movl	8(%ebp), %eax
	movl	%eax, -20(%ebp)
	.loc 1 107 0
	movl	12(%ebp), %eax
	movl	-20(%ebp), %edx
	addl	%edx, %eax
	movl	%eax, -24(%ebp)
	.loc 1 108 0
	movl	12(%ebp), %eax
	shrl	$12, %eax
	movl	%eax, -28(%ebp)
	.loc 1 109 0
	movl	$_ZL8basePage, -12(%ebp)
	jmp	.L19
.L24:
	.loc 1 111 0
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	jmp	.L20
.L23:
	.loc 1 113 0
	subl	$1, -16(%ebp)
	.loc 1 114 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	(%eax,%edx,8), %eax
	movl	%eax, -32(%ebp)
	.loc 1 115 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -36(%ebp)
	.loc 1 116 0
	movl	-24(%ebp), %eax
	cmpl	-32(%ebp), %eax
	jne	.L21
	.loc 1 119 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	leal	2(%edx), %ecx
	movl	8(%ebp), %edx
	movl	%edx, (%eax,%ecx,8)
	.loc 1 120 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %edx
	movl	-28(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	%ecx, 4(%eax,%edx,8)
	.loc 1 121 0
	jmp	.L18
.L21:
	.loc 1 123 0
	movl	-20(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jne	.L20
	.loc 1 126 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %edx
	movl	-28(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	%ecx, 4(%eax,%edx,8)
	.loc 1 127 0
	jmp	.L18
.L20:
	.loc 1 111 0 discriminator 1
	cmpl	$0, -16(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L23
	.loc 1 109 0
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -12(%ebp)
.L19:
	.loc 1 109 0 is_stmt 0 discriminator 1
	cmpl	$0, -12(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L24
	.loc 1 132 0 is_stmt 1
	movl	$_ZL8basePage, -12(%ebp)
.LBB10:
.LBB11:
	.loc 1 135 0
	jmp	.L25
.L27:
.LBB12:
.LBB13:
	.loc 1 137 0
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	cmpl	$509, %eax
	ja	.L26
.LBB14:
	.loc 1 139 0
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -40(%ebp)
	leal	1(%eax), %edx
	movl	-12(%ebp), %eax
	movl	%edx, 8(%eax)
	.loc 1 140 0
	movl	-12(%ebp), %eax
	movl	-40(%ebp), %edx
	leal	2(%edx), %ecx
	movl	8(%ebp), %edx
	movl	%edx, (%eax,%ecx,8)
	.loc 1 141 0
	movl	-28(%ebp), %edx
	movl	-12(%ebp), %eax
	movl	-40(%ebp), %ecx
	addl	$2, %ecx
	movl	%edx, 4(%eax,%ecx,8)
	.loc 1 142 0
	nop
	jmp	.L18
.L26:
.LBE14:
.LBE13:
.LBE12:
	.loc 1 135 0
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -12(%ebp)
.L25:
	.loc 1 135 0 is_stmt 0 discriminator 1
	cmpl	$0, -12(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L27
.LBE11:
	.loc 1 145 0 is_stmt 1
	call	_ZL10GetNewPagev
	.loc 1 146 0
	movl	_ZL10latestPage, %eax
	movl	%eax, -12(%ebp)
.LBE10:
	.loc 1 133 0
	nop
.LBB16:
.LBB15:
	.loc 1 135 0
	jmp	.L25
.L18:
.LBE15:
.LBE16:
.LBE9:
	.loc 1 148 0
	leave
.LCFI14:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	_ZN15CPhysicalMemory8AddChunkEPvm, .-_ZN15CPhysicalMemory8AddChunkEPvm
	.align 2
	.globl	_ZN15CPhysicalMemory11AddReservedEPvm
	.type	_ZN15CPhysicalMemory11AddReservedEPvm, @function
_ZN15CPhysicalMemory11AddReservedEPvm:
.LFB5:
	.loc 1 151 0
	.cfi_startproc
	pushl	%ebp
.LCFI15:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI16:
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$52, %esp
	.cfi_offset 3, -12
.LBB17:
	.loc 1 153 0
	movl	12(%ebp), %eax
	addl	$4095, %eax
	andl	$-4096, %eax
	movl	%eax, -20(%ebp)
	.loc 1 154 0
	movl	8(%ebp), %eax
	movl	%eax, -24(%ebp)
	.loc 1 155 0
	movl	-20(%ebp), %eax
	sall	$12, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -28(%ebp)
.LBB18:
	.loc 1 156 0
	movl	$_ZL8basePage, -12(%ebp)
	jmp	.L30
.L37:
.LBB19:
.LBB20:
	.loc 1 158 0
	movl	$0, -16(%ebp)
	jmp	.L31
.L36:
.LBB21:
	.loc 1 160 0
	movl	-16(%ebp), %eax
	addl	$2, %eax
	leal	0(,%eax,8), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -32(%ebp)
	.loc 1 161 0
	movl	-32(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -36(%ebp)
	.loc 1 162 0
	movl	-32(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -40(%ebp)
	.loc 1 164 0
	movl	-24(%ebp), %eax
	cmpl	-36(%ebp), %eax
	ja	.L32
	.loc 1 166 0
	movl	-28(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jb	.L33
	.loc 1 170 0
	movl	-28(%ebp), %eax
	cmpl	-40(%ebp), %eax
	jbe	.L34
	.loc 1 173 0
	movl	-32(%ebp), %eax
	movl	$0, 4(%eax)
	jmp	.L33
.L34:
	.loc 1 178 0
	movl	-32(%ebp), %eax
	movl	-28(%ebp), %edx
	movl	%edx, (%eax)
	.loc 1 179 0
	movl	-32(%ebp), %eax
	movl	4(%eax), %edx
	movl	-28(%ebp), %ecx
	movl	-36(%ebp), %eax
	movl	%ecx, %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	sarl	$12, %eax
	subl	%eax, %edx
	movl	-32(%ebp), %eax
	movl	%edx, 4(%eax)
	jmp	.L33
.L32:
	.loc 1 184 0
	movl	-24(%ebp), %eax
	cmpl	-40(%ebp), %eax
	ja	.L33
	.loc 1 188 0
	movl	-28(%ebp), %eax
	cmpl	-40(%ebp), %eax
	jae	.L35
	.loc 1 191 0
	movl	-40(%ebp), %edx
	movl	-24(%ebp), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	sarl	$12, %eax
	movl	%eax, %edx
	movl	-32(%ebp), %eax
	movl	%edx, 4(%eax)
	.loc 1 192 0
	movl	-40(%ebp), %edx
	movl	-28(%ebp), %eax
	movl	%edx, %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	%eax, 4(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory8AddChunkEPvm
	jmp	.L33
.L35:
	.loc 1 197 0
	movl	-40(%ebp), %edx
	movl	-24(%ebp), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	sarl	$12, %eax
	movl	%eax, %edx
	movl	-32(%ebp), %eax
	movl	%edx, 4(%eax)
.L33:
.LBE21:
	.loc 1 158 0
	addl	$1, -16(%ebp)
.L31:
	.loc 1 158 0 is_stmt 0 discriminator 1
	movl	-16(%ebp), %edx
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	cmpl	%eax, %edx
	setb	%al
	testb	%al, %al
	jne	.L36
.LBE20:
.LBE19:
	.loc 1 156 0 is_stmt 1
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -12(%ebp)
.L30:
	.loc 1 156 0 is_stmt 0 discriminator 1
	cmpl	$0, -12(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L37
.LBE18:
	.loc 1 203 0 is_stmt 1
	call	_ZL9DropPagesv
.LBE17:
	.loc 1 204 0
	addl	$52, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI17:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	_ZN15CPhysicalMemory11AddReservedEPvm, .-_ZN15CPhysicalMemory11AddReservedEPvm
	.align 2
	.globl	_ZN15CPhysicalMemory10TotalPagesEv
	.type	_ZN15CPhysicalMemory10TotalPagesEv, @function
_ZN15CPhysicalMemory10TotalPagesEv:
.LFB6:
	.loc 1 207 0
	.cfi_startproc
	pushl	%ebp
.LCFI18:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI19:
	.cfi_def_cfa_register 5
	subl	$16, %esp
.LBB22:
	.loc 1 210 0
	movl	$0, -4(%ebp)
.LBB23:
	.loc 1 211 0
	movl	$_ZL8basePage, -8(%ebp)
	jmp	.L39
.L42:
.LBB24:
.LBB25:
	.loc 1 213 0
	movl	$0, -12(%ebp)
	jmp	.L40
.L41:
	.loc 1 215 0 discriminator 2
	movl	-8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	addl	%eax, -4(%ebp)
	.loc 1 213 0 discriminator 2
	addl	$1, -12(%ebp)
.L40:
	.loc 1 213 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %edx
	movl	-8(%ebp), %eax
	movl	8(%eax), %eax
	cmpl	%eax, %edx
	setb	%al
	testb	%al, %al
	jne	.L41
.LBE25:
.LBE24:
	.loc 1 211 0 is_stmt 1
	movl	-8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -8(%ebp)
.L39:
	.loc 1 211 0 is_stmt 0 discriminator 1
	cmpl	$0, -8(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L42
.LBE23:
	.loc 1 218 0 is_stmt 1
	movl	-4(%ebp), %eax
.LBE22:
	.loc 1 219 0
	leave
.LCFI20:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	_ZN15CPhysicalMemory10TotalPagesEv, .-_ZN15CPhysicalMemory10TotalPagesEv
	.align 2
	.globl	_ZN15CPhysicalMemory9TotalUsedEv
	.type	_ZN15CPhysicalMemory9TotalUsedEv, @function
_ZN15CPhysicalMemory9TotalUsedEv:
.LFB7:
	.loc 1 222 0
	.cfi_startproc
	pushl	%ebp
.LCFI21:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI22:
	.cfi_def_cfa_register 5
	.loc 1 223 0
	movl	_ZL9usedPages, %eax
	.loc 1 224 0
	popl	%ebp
.LCFI23:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	_ZN15CPhysicalMemory9TotalUsedEv, .-_ZN15CPhysicalMemory9TotalUsedEv
	.align 2
	.globl	_ZN15CPhysicalMemory12PageToMemoryEm
	.type	_ZN15CPhysicalMemory12PageToMemoryEm, @function
_ZN15CPhysicalMemory12PageToMemoryEm:
.LFB8:
	.loc 1 227 0
	.cfi_startproc
	pushl	%ebp
.LCFI24:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI25:
	.cfi_def_cfa_register 5
	.loc 1 228 0
	movl	8(%ebp), %eax
	sall	$12, %eax
	.loc 1 229 0
	popl	%ebp
.LCFI26:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	_ZN15CPhysicalMemory12PageToMemoryEm, .-_ZN15CPhysicalMemory12PageToMemoryEm
	.align 2
	.globl	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	.type	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_, @function
_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_:
.LFB9:
	.loc 1 232 0
	.cfi_startproc
	pushl	%ebp
.LCFI27:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI28:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB26:
.LBB27:
	.loc 1 236 0
	movl	_ZL10latestPage, %eax
	movl	%eax, -12(%ebp)
	jmp	.L49
.L54:
.LBB28:
.LBB29:
	.loc 1 238 0
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	jmp	.L50
.L53:
.LBB30:
	.loc 1 240 0
	subl	$1, -16(%ebp)
.LBB31:
	.loc 1 241 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	cmpl	8(%ebp), %eax
	jb	.L50
	.loc 1 241 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	(%eax,%edx,8), %eax
	cmpl	12(%ebp), %eax
	jb	.L50
	.loc 1 241 0 discriminator 2
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	(%eax,%edx,8), %edx
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %ecx
	addl	$2, %ecx
	movl	4(%eax,%ecx,8), %eax
	addl	%edx, %eax
	cmpl	16(%ebp), %eax
	jae	.L50
.LBB32:
	.loc 1 246 0 is_stmt 1
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	(%eax,%edx,8), %eax
	movl	%eax, -20(%ebp)
	.loc 1 247 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	(%eax,%edx,8), %eax
	movl	8(%ebp), %edx
	sall	$12, %edx
	leal	(%eax,%edx), %ecx
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	%ecx, (%eax,%edx,8)
	.loc 1 248 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	movl	%eax, %edx
	subl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %ecx
	addl	$2, %ecx
	movl	%edx, 4(%eax,%ecx,8)
	.loc 1 249 0
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$2, %edx
	movl	4(%eax,%edx,8), %eax
	testl	%eax, %eax
	jne	.L51
	.loc 1 250 0
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZL8TidyPageP10PAGE_STACK
.L51:
	.loc 1 251 0
	movl	_ZL9usedPages, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, _ZL9usedPages
	.loc 1 252 0
	movl	-20(%ebp), %eax
	jmp	.L52
.L50:
.LBE32:
.LBE31:
.LBE30:
	.loc 1 238 0 discriminator 1
	cmpl	$0, -16(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L53
.LBE29:
.LBE28:
	.loc 1 236 0
	movl	-12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -12(%ebp)
.L49:
	.loc 1 236 0 is_stmt 0 discriminator 1
	cmpl	$0, -12(%ebp)
	setne	%al
	testb	%al, %al
	jne	.L54
.LBE27:
	.loc 1 256 0 is_stmt 1
	movl	$-1, %eax
.L52:
.LBE26:
	.loc 1 257 0
	leave
.LCFI29:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_, .-_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	.align 2
	.globl	_ZN15CPhysicalMemory12ReleasePagesEPvm
	.type	_ZN15CPhysicalMemory12ReleasePagesEPvm, @function
_ZN15CPhysicalMemory12ReleasePagesEPvm:
.LFB10:
	.loc 1 260 0
	.cfi_startproc
	pushl	%ebp
.LCFI30:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI31:
	.cfi_def_cfa_register 5
	subl	$24, %esp
	.loc 1 262 0
	movl	12(%ebp), %eax
	sall	$12, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory8AddChunkEPvm
	.loc 1 263 0
	movl	_ZL9usedPages, %eax
	subl	12(%ebp), %eax
	movl	%eax, _ZL9usedPages
	.loc 1 264 0
	leave
.LCFI32:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.size	_ZN15CPhysicalMemory12ReleasePagesEPvm, .-_ZN15CPhysicalMemory12ReleasePagesEPvm
.Letext0:
	.file 2 "runtime.h"
	.file 3 "mem_physical.h"
	.file 4 "mem_logical.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0xac8
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF118
	.byte	0x4
	.long	.LASF119
	.long	.LASF120
	.long	.Ltext0
	.long	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF3
	.byte	0x2
	.byte	0x8
	.long	0x30
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF2
	.uleb128 0x2
	.long	.LASF4
	.byte	0x2
	.byte	0xc
	.long	0x30
	.uleb128 0x2
	.long	.LASF5
	.byte	0x3
	.byte	0x6
	.long	0x5b
	.uleb128 0x4
	.byte	0x4
	.uleb128 0x2
	.long	.LASF6
	.byte	0x3
	.byte	0x7
	.long	0x68
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF7
	.uleb128 0x2
	.long	.LASF8
	.byte	0x3
	.byte	0x8
	.long	0x68
	.uleb128 0x5
	.long	.LASF54
	.byte	0x1
	.byte	0x3
	.byte	0xa
	.long	0x16d
	.uleb128 0x6
	.long	.LASF9
	.byte	0x3
	.byte	0xe
	.long	0x16d
	.byte	0x1
	.byte	0x1
	.uleb128 0x6
	.long	.LASF10
	.byte	0x3
	.byte	0xf
	.long	0x16d
	.byte	0x1
	.byte	0x1
	.uleb128 0x6
	.long	.LASF11
	.byte	0x3
	.byte	0x10
	.long	0x16d
	.byte	0x1
	.byte	0x1
	.uleb128 0x7
	.byte	0x1
	.long	.LASF63
	.byte	0x3
	.byte	0x13
	.long	.LASF121
	.byte	0x1
	.uleb128 0x8
	.byte	0x1
	.long	.LASF12
	.byte	0x3
	.byte	0x14
	.long	.LASF14
	.byte	0x1
	.long	0xd6
	.uleb128 0x9
	.long	0x50
	.uleb128 0x9
	.long	0x5d
	.byte	0
	.uleb128 0x8
	.byte	0x1
	.long	.LASF13
	.byte	0x3
	.byte	0x15
	.long	.LASF15
	.byte	0x1
	.long	0xf2
	.uleb128 0x9
	.long	0x50
	.uleb128 0x9
	.long	0x5d
	.byte	0
	.uleb128 0xa
	.byte	0x1
	.long	.LASF16
	.byte	0x3
	.byte	0x16
	.long	.LASF18
	.long	0x6f
	.byte	0x1
	.uleb128 0xa
	.byte	0x1
	.long	.LASF17
	.byte	0x3
	.byte	0x17
	.long	.LASF19
	.long	0x6f
	.byte	0x1
	.uleb128 0xb
	.byte	0x1
	.long	.LASF20
	.byte	0x3
	.byte	0x18
	.long	.LASF22
	.long	0x5d
	.byte	0x1
	.long	0x12f
	.uleb128 0x9
	.long	0x6f
	.byte	0
	.uleb128 0xb
	.byte	0x1
	.long	.LASF21
	.byte	0x3
	.byte	0x1b
	.long	.LASF23
	.long	0x50
	.byte	0x1
	.long	0x154
	.uleb128 0x9
	.long	0x6f
	.uleb128 0x9
	.long	0x50
	.uleb128 0x9
	.long	0x50
	.byte	0
	.uleb128 0xc
	.byte	0x1
	.long	.LASF122
	.byte	0x3
	.byte	0x1c
	.long	.LASF123
	.byte	0x1
	.uleb128 0x9
	.long	0x50
	.uleb128 0x9
	.long	0x6f
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	0x50
	.uleb128 0xe
	.byte	0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF27
	.long	0x18b
	.uleb128 0xf
	.long	.LASF24
	.sleb128 0
	.uleb128 0xf
	.long	.LASF25
	.sleb128 1
	.byte	0
	.uleb128 0x2
	.long	.LASF26
	.byte	0x4
	.byte	0xa
	.long	0x172
	.uleb128 0xe
	.byte	0x4
	.byte	0x4
	.byte	0xc
	.long	.LASF28
	.long	0x1b5
	.uleb128 0xf
	.long	.LASF29
	.sleb128 1
	.uleb128 0xf
	.long	.LASF30
	.sleb128 2
	.uleb128 0xf
	.long	.LASF31
	.sleb128 4
	.byte	0
	.uleb128 0x2
	.long	.LASF32
	.byte	0x4
	.byte	0x10
	.long	0x196
	.uleb128 0x10
	.byte	0x4
	.byte	0x4
	.byte	0x12
	.long	.LASF45
	.long	0x2b2
	.uleb128 0x11
	.byte	0x4
	.byte	0x4
	.byte	0x13
	.long	0x2a9
	.uleb128 0x12
	.byte	0x4
	.byte	0x4
	.byte	0x14
	.long	0x298
	.uleb128 0x13
	.long	.LASF33
	.byte	0x4
	.byte	0x15
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF34
	.byte	0x4
	.byte	0x16
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF35
	.byte	0x4
	.byte	0x17
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF36
	.byte	0x4
	.byte	0x18
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF37
	.byte	0x4
	.byte	0x19
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF38
	.byte	0x4
	.byte	0x1a
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF39
	.byte	0x4
	.byte	0x1b
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF40
	.byte	0x4
	.byte	0x1c
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF41
	.byte	0x4
	.byte	0x1d
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF42
	.byte	0x4
	.byte	0x1e
	.long	0x45
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF43
	.byte	0x4
	.byte	0x1f
	.long	0x45
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x14
	.long	0x1d4
	.uleb128 0x15
	.long	.LASF50
	.byte	0x4
	.byte	0x21
	.long	0x45
	.byte	0
	.uleb128 0x16
	.long	0x1cc
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF44
	.byte	0x4
	.byte	0x23
	.long	0x1c0
	.uleb128 0x10
	.byte	0x4
	.byte	0x4
	.byte	0x25
	.long	.LASF46
	.long	0x3af
	.uleb128 0x11
	.byte	0x4
	.byte	0x4
	.byte	0x26
	.long	0x3a6
	.uleb128 0x12
	.byte	0x4
	.byte	0x4
	.byte	0x27
	.long	0x395
	.uleb128 0x13
	.long	.LASF33
	.byte	0x4
	.byte	0x28
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF34
	.byte	0x4
	.byte	0x29
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF35
	.byte	0x4
	.byte	0x2a
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF47
	.byte	0x4
	.byte	0x2b
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF37
	.byte	0x4
	.byte	0x2c
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF38
	.byte	0x4
	.byte	0x2d
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF48
	.byte	0x4
	.byte	0x2e
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF49
	.byte	0x4
	.byte	0x2f
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF41
	.byte	0x4
	.byte	0x30
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF42
	.byte	0x4
	.byte	0x31
	.long	0x45
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x13
	.long	.LASF43
	.byte	0x4
	.byte	0x32
	.long	0x45
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x14
	.long	0x2d1
	.uleb128 0x15
	.long	.LASF50
	.byte	0x4
	.byte	0x34
	.long	0x45
	.byte	0
	.uleb128 0x16
	.long	0x2c9
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF51
	.byte	0x4
	.byte	0x36
	.long	0x2bd
	.uleb128 0x17
	.value	0x1000
	.byte	0x4
	.byte	0x38
	.long	.LASF124
	.long	0x3d6
	.uleb128 0x18
	.long	.LASF56
	.byte	0x4
	.byte	0x39
	.long	0x3d6
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x19
	.long	0x3af
	.long	0x3e7
	.uleb128 0x1a
	.long	0x3e7
	.value	0x3ff
	.byte	0
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF52
	.uleb128 0x2
	.long	.LASF53
	.byte	0x4
	.byte	0x3a
	.long	0x3ba
	.uleb128 0x1b
	.long	.LASF55
	.value	0x100c
	.byte	0x4
	.byte	0x3c
	.long	0x598
	.uleb128 0x18
	.long	.LASF57
	.byte	0x4
	.byte	0x40
	.long	0x598
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x18
	.long	.LASF58
	.byte	0x4
	.byte	0x41
	.long	0x5af
	.byte	0x3
	.byte	0x23
	.uleb128 0x1000
	.uleb128 0x18
	.long	.LASF59
	.byte	0x4
	.byte	0x42
	.long	0x50
	.byte	0x3
	.byte	0x23
	.uleb128 0x1004
	.uleb128 0x1c
	.long	.LASF60
	.byte	0x4
	.byte	0x53
	.long	0x5b5
	.byte	0x3
	.byte	0x23
	.uleb128 0x1008
	.byte	0x3
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF61
	.byte	0x4
	.byte	0x44
	.long	.LASF62
	.byte	0x1
	.long	0x457
	.long	0x45e
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF63
	.byte	0x4
	.byte	0x46
	.long	.LASF64
	.byte	0x1
	.long	0x473
	.long	0x47a
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF65
	.byte	0x4
	.byte	0x47
	.long	.LASF66
	.byte	0x1
	.long	0x48f
	.long	0x496
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x4
	.byte	0x48
	.long	.LASF68
	.byte	0x1
	.long	0x4ab
	.long	0x4b2
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.byte	0
	.uleb128 0x1f
	.byte	0x1
	.string	"Map"
	.byte	0x4
	.byte	0x4a
	.long	.LASF125
	.long	0x5b
	.byte	0x1
	.long	0x4cb
	.long	0x4e6
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.uleb128 0x9
	.long	0x5b5
	.uleb128 0x9
	.long	0x18b
	.uleb128 0x9
	.long	0x50
	.uleb128 0x9
	.long	0x25
	.byte	0
	.uleb128 0x20
	.byte	0x1
	.string	"Map"
	.byte	0x4
	.byte	0x4b
	.long	.LASF126
	.byte	0x1
	.long	0x4fb
	.long	0x511
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.uleb128 0x9
	.long	0x5b5
	.uleb128 0x9
	.long	0x5b
	.uleb128 0x9
	.long	0x50
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF69
	.byte	0x4
	.byte	0x4c
	.long	.LASF70
	.byte	0x1
	.long	0x526
	.long	0x532
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.uleb128 0x9
	.long	0x5b
	.byte	0
	.uleb128 0x21
	.byte	0x1
	.long	.LASF71
	.byte	0x4
	.byte	0x4d
	.long	.LASF72
	.long	0x50
	.byte	0x1
	.long	0x54b
	.long	0x557
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.uleb128 0x9
	.long	0x5b
	.byte	0
	.uleb128 0x22
	.byte	0x1
	.long	.LASF69
	.byte	0x4
	.byte	0x50
	.long	.LASF73
	.byte	0x3
	.byte	0x1
	.long	0x56d
	.long	0x57e
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.uleb128 0x9
	.long	0x5b
	.uleb128 0x9
	.long	0x5c2
	.byte	0
	.uleb128 0x23
	.byte	0x1
	.long	.LASF74
	.byte	0x4
	.byte	0x51
	.long	.LASF75
	.byte	0x3
	.byte	0x1
	.long	0x590
	.uleb128 0x1e
	.long	0x5bc
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x19
	.long	0x5a9
	.long	0x5a9
	.uleb128 0x1a
	.long	0x3e7
	.value	0x3ff
	.byte	0
	.uleb128 0x24
	.byte	0x4
	.long	0x3ee
	.uleb128 0x24
	.byte	0x4
	.long	0x2b2
	.uleb128 0x25
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x24
	.byte	0x4
	.long	0x3f9
	.uleb128 0x3
	.byte	0x1
	.byte	0x2
	.long	.LASF76
	.uleb128 0x10
	.byte	0x8
	.byte	0x1
	.byte	0x9
	.long	.LASF77
	.long	0x5f2
	.uleb128 0x18
	.long	.LASF78
	.byte	0x1
	.byte	0xb
	.long	0x5b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x18
	.long	.LASF79
	.byte	0x1
	.byte	0xc
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0
	.uleb128 0x2
	.long	.LASF80
	.byte	0x1
	.byte	0xe
	.long	0x5c9
	.uleb128 0x26
	.long	.LASF86
	.value	0x1000
	.byte	0x1
	.byte	0x10
	.long	0x651
	.uleb128 0x18
	.long	.LASF81
	.byte	0x1
	.byte	0x12
	.long	0x651
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x18
	.long	.LASF82
	.byte	0x1
	.byte	0x13
	.long	0x651
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x18
	.long	.LASF83
	.byte	0x1
	.byte	0x14
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x18
	.long	.LASF84
	.byte	0x1
	.byte	0x15
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x18
	.long	.LASF85
	.byte	0x1
	.byte	0x16
	.long	0x657
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0
	.uleb128 0x24
	.byte	0x4
	.long	0x5fd
	.uleb128 0x19
	.long	0x5f2
	.long	0x668
	.uleb128 0x1a
	.long	0x3e7
	.value	0x1fd
	.byte	0
	.uleb128 0x2
	.long	.LASF86
	.byte	0x1
	.byte	0x18
	.long	0x5fd
	.uleb128 0x27
	.long	0xad
	.byte	0x1
	.byte	0x1e
	.long	.LFB0
	.long	.LFE0
	.long	.LLST0
	.byte	0x1
	.uleb128 0x28
	.long	.LASF89
	.byte	0x1
	.byte	0x27
	.long	.LFB1
	.long	.LFE1
	.long	.LLST1
	.byte	0x1
	.long	0x6c6
	.uleb128 0x29
	.long	.LBB2
	.long	.LBE2
	.uleb128 0x2a
	.long	.LASF87
	.byte	0x1
	.byte	0x29
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2a
	.long	.LASF88
	.byte	0x1
	.byte	0x2a
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x24
	.byte	0x4
	.long	0x668
	.uleb128 0x28
	.long	.LASF90
	.byte	0x1
	.byte	0x32
	.long	.LFB2
	.long	.LFE2
	.long	.LLST2
	.byte	0x1
	.long	0x729
	.uleb128 0x2b
	.long	.LASF95
	.byte	0x1
	.byte	0x32
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x29
	.long	.LBB3
	.long	.LBE3
	.uleb128 0x2a
	.long	.LASF91
	.byte	0x1
	.byte	0x52
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2c
	.long	.Ldebug_ranges0+0
	.uleb128 0x2d
	.string	"x"
	.byte	0x1
	.byte	0x37
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2d
	.string	"y"
	.byte	0x1
	.byte	0x37
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x28
	.long	.LASF92
	.byte	0x1
	.byte	0x57
	.long	.LFB3
	.long	.LFE3
	.long	.LLST3
	.byte	0x1
	.long	0x768
	.uleb128 0x29
	.long	.LBB8
	.long	.LBE8
	.uleb128 0x2a
	.long	.LASF93
	.byte	0x1
	.byte	0x59
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2a
	.long	.LASF94
	.byte	0x1
	.byte	0x59
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x2e
	.long	0xba
	.byte	0x1
	.byte	0x63
	.long	.LFB4
	.long	.LFE4
	.long	.LLST4
	.byte	0x1
	.long	0x821
	.uleb128 0x2b
	.long	.LASF96
	.byte	0x1
	.byte	0x63
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2b
	.long	.LASF97
	.byte	0x1
	.byte	0x63
	.long	0x5d
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x29
	.long	.LBB9
	.long	.LBE9
	.uleb128 0x2a
	.long	.LASF93
	.byte	0x1
	.byte	0x65
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2a
	.long	.LASF98
	.byte	0x1
	.byte	0x66
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x2a
	.long	.LASF79
	.byte	0x1
	.byte	0x66
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x2a
	.long	.LASF99
	.byte	0x1
	.byte	0x67
	.long	0x821
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2a
	.long	.LASF100
	.byte	0x1
	.byte	0x67
	.long	0x821
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2a
	.long	.LASF101
	.byte	0x1
	.byte	0x68
	.long	0x821
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x2a
	.long	.LASF102
	.byte	0x1
	.byte	0x68
	.long	0x821
	.byte	0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x29
	.long	.LBB14
	.long	.LBE14
	.uleb128 0x2a
	.long	.LASF103
	.byte	0x1
	.byte	0x8b
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -48
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x24
	.byte	0x4
	.long	0x37
	.uleb128 0x2e
	.long	0xd6
	.byte	0x1
	.byte	0x96
	.long	.LFB5
	.long	.LFE5
	.long	.LLST5
	.byte	0x1
	.long	0x8f2
	.uleb128 0x2b
	.long	.LASF96
	.byte	0x1
	.byte	0x96
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2b
	.long	.LASF97
	.byte	0x1
	.byte	0x96
	.long	0x5d
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x29
	.long	.LBB17
	.long	.LBE17
	.uleb128 0x2a
	.long	.LASF79
	.byte	0x1
	.byte	0x99
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2a
	.long	.LASF104
	.byte	0x1
	.byte	0x9a
	.long	0x8f2
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2a
	.long	.LASF105
	.byte	0x1
	.byte	0x9b
	.long	0x8f2
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x29
	.long	.LBB18
	.long	.LBE18
	.uleb128 0x2a
	.long	.LASF93
	.byte	0x1
	.byte	0x9c
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x29
	.long	.LBB20
	.long	.LBE20
	.uleb128 0x2d
	.string	"i"
	.byte	0x1
	.byte	0x9e
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x29
	.long	.LBB21
	.long	.LBE21
	.uleb128 0x2a
	.long	.LASF106
	.byte	0x1
	.byte	0xa0
	.long	0x8ff
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x2a
	.long	.LASF107
	.byte	0x1
	.byte	0xa1
	.long	0x8f2
	.byte	0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x2a
	.long	.LASF108
	.byte	0x1
	.byte	0xa2
	.long	0x8f2
	.byte	0x2
	.byte	0x91
	.sleb128 -48
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x24
	.byte	0x4
	.long	0x8f8
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF109
	.uleb128 0x24
	.byte	0x4
	.long	0x5f2
	.uleb128 0x2f
	.long	0xf2
	.byte	0x1
	.byte	0xce
	.long	.LFB6
	.long	.LFE6
	.long	.LLST6
	.byte	0x1
	.long	0x964
	.uleb128 0x29
	.long	.LBB22
	.long	.LBE22
	.uleb128 0x2a
	.long	.LASF110
	.byte	0x1
	.byte	0xd0
	.long	0x6f
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x29
	.long	.LBB23
	.long	.LBE23
	.uleb128 0x2a
	.long	.LASF93
	.byte	0x1
	.byte	0xd3
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x29
	.long	.LBB25
	.long	.LBE25
	.uleb128 0x2d
	.string	"i"
	.byte	0x1
	.byte	0xd5
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x27
	.long	0x103
	.byte	0x1
	.byte	0xdd
	.long	.LFB7
	.long	.LFE7
	.long	.LLST7
	.byte	0x1
	.uleb128 0x2f
	.long	0x114
	.byte	0x1
	.byte	0xe2
	.long	.LFB8
	.long	.LFE8
	.long	.LLST8
	.byte	0x1
	.long	0x99f
	.uleb128 0x2b
	.long	.LASF110
	.byte	0x1
	.byte	0xe2
	.long	0x6f
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.long	0x12f
	.byte	0x1
	.byte	0xe7
	.long	.LFB9
	.long	.LFE9
	.long	.LLST9
	.byte	0x1
	.long	0xa20
	.uleb128 0x2b
	.long	.LASF110
	.byte	0x1
	.byte	0xe7
	.long	0x6f
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x30
	.string	"min"
	.byte	0x1
	.byte	0xe7
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x30
	.string	"max"
	.byte	0x1
	.byte	0xe7
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x29
	.long	.LBB26
	.long	.LBE26
	.uleb128 0x2a
	.long	.LASF93
	.byte	0x1
	.byte	0xe9
	.long	0x6c6
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2a
	.long	.LASF98
	.byte	0x1
	.byte	0xea
	.long	0x5b5
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x29
	.long	.LBB32
	.long	.LBE32
	.uleb128 0x2a
	.long	.LASF111
	.byte	0x1
	.byte	0xf4
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x31
	.long	0x154
	.byte	0x1
	.value	0x103
	.long	.LFB10
	.long	.LFE10
	.long	.LLST10
	.byte	0x1
	.long	0xa58
	.uleb128 0x32
	.long	.LASF96
	.byte	0x1
	.value	0x103
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x32
	.long	.LASF110
	.byte	0x1
	.value	0x103
	.long	0x6f
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.byte	0
	.uleb128 0x33
	.long	.LASF127
	.byte	0x4
	.byte	0x57
	.long	0x5bc
	.byte	0x1
	.byte	0x1
	.uleb128 0x2a
	.long	.LASF112
	.byte	0x1
	.byte	0x1a
	.long	0x668
	.byte	0x5
	.byte	0x3
	.long	_ZL8basePage
	.uleb128 0x2a
	.long	.LASF113
	.byte	0x1
	.byte	0x1b
	.long	0x6c6
	.byte	0x5
	.byte	0x3
	.long	_ZL10latestPage
	.uleb128 0x2a
	.long	.LASF114
	.byte	0x1
	.byte	0x1c
	.long	0x30
	.byte	0x5
	.byte	0x3
	.long	_ZL9usedPages
	.uleb128 0x34
	.long	0x86
	.byte	0x1
	.byte	0x4
	.long	.LASF115
	.byte	0x5
	.byte	0x3
	.long	_ZN15CPhysicalMemory7MinimumE
	.uleb128 0x34
	.long	0x93
	.byte	0x1
	.byte	0x5
	.long	.LASF116
	.byte	0x5
	.byte	0x3
	.long	_ZN15CPhysicalMemory7MaximumE
	.uleb128 0x34
	.long	0xa0
	.byte	0x1
	.byte	0x6
	.long	.LASF117
	.byte	0x5
	.byte	0x3
	.long	_ZN15CPhysicalMemory7InvalidE
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x2
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0xd
	.uleb128 0xb
	.uleb128 0xc
	.uleb128 0xb
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x2
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x32
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x32
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x32
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x2e
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x30
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x31
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x32
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x33
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x34
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.long	.LFB0-.Ltext0
	.long	.LCFI0-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI0-.Ltext0
	.long	.LCFI1-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI1-.Ltext0
	.long	.LCFI2-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI2-.Ltext0
	.long	.LFE0-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST1:
	.long	.LFB1-.Ltext0
	.long	.LCFI3-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI3-.Ltext0
	.long	.LCFI4-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI4-.Ltext0
	.long	.LCFI5-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI5-.Ltext0
	.long	.LFE1-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST2:
	.long	.LFB2-.Ltext0
	.long	.LCFI6-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI6-.Ltext0
	.long	.LCFI7-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI7-.Ltext0
	.long	.LCFI8-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI8-.Ltext0
	.long	.LFE2-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST3:
	.long	.LFB3-.Ltext0
	.long	.LCFI9-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI9-.Ltext0
	.long	.LCFI10-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI10-.Ltext0
	.long	.LCFI11-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI11-.Ltext0
	.long	.LFE3-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST4:
	.long	.LFB4-.Ltext0
	.long	.LCFI12-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI12-.Ltext0
	.long	.LCFI13-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI13-.Ltext0
	.long	.LCFI14-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI14-.Ltext0
	.long	.LFE4-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST5:
	.long	.LFB5-.Ltext0
	.long	.LCFI15-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI15-.Ltext0
	.long	.LCFI16-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI16-.Ltext0
	.long	.LCFI17-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI17-.Ltext0
	.long	.LFE5-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST6:
	.long	.LFB6-.Ltext0
	.long	.LCFI18-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI18-.Ltext0
	.long	.LCFI19-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI19-.Ltext0
	.long	.LCFI20-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI20-.Ltext0
	.long	.LFE6-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST7:
	.long	.LFB7-.Ltext0
	.long	.LCFI21-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI21-.Ltext0
	.long	.LCFI22-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI22-.Ltext0
	.long	.LCFI23-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI23-.Ltext0
	.long	.LFE7-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST8:
	.long	.LFB8-.Ltext0
	.long	.LCFI24-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI24-.Ltext0
	.long	.LCFI25-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI25-.Ltext0
	.long	.LCFI26-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI26-.Ltext0
	.long	.LFE8-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST9:
	.long	.LFB9-.Ltext0
	.long	.LCFI27-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI27-.Ltext0
	.long	.LCFI28-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI28-.Ltext0
	.long	.LCFI29-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI29-.Ltext0
	.long	.LFE9-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST10:
	.long	.LFB10-.Ltext0
	.long	.LCFI30-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI30-.Ltext0
	.long	.LCFI31-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI31-.Ltext0
	.long	.LCFI32-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI32-.Ltext0
	.long	.LFE10-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.LBB5-.Ltext0
	.long	.LBE5-.Ltext0
	.long	.LBB6-.Ltext0
	.long	.LBE6-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF47:
	.string	"WriteThough"
.LASF111:
	.string	"result"
.LASF112:
	.string	"basePage"
.LASF22:
	.string	"_ZN15CPhysicalMemory12PageToMemoryEm"
.LASF117:
	.string	"_ZN15CPhysicalMemory7InvalidE"
.LASF82:
	.string	"nextPage"
.LASF125:
	.string	"_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj"
.LASF122:
	.string	"ReleasePages"
.LASF69:
	.string	"Unmap"
.LASF64:
	.string	"_ZN18SPageDirectoryInfo4InitEv"
.LASF3:
	.string	"size_t"
.LASF50:
	.string	"RawValue"
.LASF52:
	.string	"sizetype"
.LASF59:
	.string	"directoryPhysical"
.LASF61:
	.string	"InitKernel"
.LASF123:
	.string	"_ZN15CPhysicalMemory12ReleasePagesEPvm"
.LASF87:
	.string	"newPage"
.LASF57:
	.string	"tables"
.LASF39:
	.string	"Available"
.LASF90:
	.string	"TidyPage"
.LASF91:
	.string	"oldPage"
.LASF71:
	.string	"Address"
.LASF94:
	.string	"next"
.LASF83:
	.string	"entryCount"
.LASF31:
	.string	"fmPageData"
.LASF73:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPvb"
.LASF85:
	.string	"entries"
.LASF127:
	.string	"currentAddressSpace"
.LASF101:
	.string	"curStart"
.LASF27:
	.string	"8MAP_TYPE"
.LASF98:
	.string	"index"
.LASF113:
	.string	"latestPage"
.LASF78:
	.string	"pageBase"
.LASF110:
	.string	"count"
.LASF26:
	.string	"MAP_TYPE"
.LASF76:
	.string	"bool"
.LASF36:
	.string	"WriteThrough"
.LASF116:
	.string	"_ZN15CPhysicalMemory7MaximumE"
.LASF126:
	.string	"_ZN18SPageDirectoryInfo3MapEiPvS0_"
.LASF104:
	.string	"reserveLower"
.LASF88:
	.string	"newObject"
.LASF42:
	.string	"UserData"
.LASF30:
	.string	"fmUser"
.LASF49:
	.string	"PatIndex"
.LASF54:
	.string	"CPhysicalMemory"
.LASF63:
	.string	"Init"
.LASF16:
	.string	"TotalPages"
.LASF95:
	.string	"page"
.LASF24:
	.string	"pmApplication"
.LASF96:
	.string	"address"
.LASF60:
	.string	"revision"
.LASF67:
	.string	"Select"
.LASF43:
	.string	"Frame"
.LASF9:
	.string	"Minimum"
.LASF37:
	.string	"CacheDisable"
.LASF32:
	.string	"MAP_FLAGS"
.LASF118:
	.string	"GNU C++ 4.7.1 20120721 (prerelease)"
.LASF62:
	.string	"_ZN18SPageDirectoryInfo10InitKernelEv"
.LASF58:
	.string	"tablesPhysical"
.LASF41:
	.string	"Global"
.LASF21:
	.string	"AllocateContiguousPages"
.LASF46:
	.string	"5SPage"
.LASF10:
	.string	"Maximum"
.LASF120:
	.string	"/home/colin/Development/OSdevelopment/Kernel/Kernel/temp"
.LASF106:
	.string	"entry"
.LASF102:
	.string	"curEnd"
.LASF1:
	.string	"unsigned char"
.LASF45:
	.string	"14SPageDirectory"
.LASF33:
	.string	"Present"
.LASF20:
	.string	"PageToMemory"
.LASF17:
	.string	"TotalUsed"
.LASF89:
	.string	"GetNewPage"
.LASF84:
	.string	"reserved"
.LASF105:
	.string	"reserveUpper"
.LASF86:
	.string	"PAGE_STACK"
.LASF0:
	.string	"unsigned int"
.LASF51:
	.string	"SPage"
.LASF8:
	.string	"PageCount"
.LASF124:
	.string	"10SPageTable"
.LASF93:
	.string	"current"
.LASF66:
	.string	"_ZN18SPageDirectoryInfo7ReleaseEv"
.LASF100:
	.string	"newEnd"
.LASF2:
	.string	"short unsigned int"
.LASF75:
	.string	"_ZN18SPageDirectoryInfo10CopyKernelEv"
.LASF109:
	.string	"char"
.LASF114:
	.string	"usedPages"
.LASF115:
	.string	"_ZN15CPhysicalMemory7MinimumE"
.LASF107:
	.string	"entryLower"
.LASF79:
	.string	"pageCount"
.LASF70:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPv"
.LASF81:
	.string	"lastPage"
.LASF121:
	.string	"_ZN15CPhysicalMemory4InitEv"
.LASF6:
	.string	"PhysicalLength"
.LASF4:
	.string	"UInt32"
.LASF7:
	.string	"long unsigned int"
.LASF72:
	.string	"_ZN18SPageDirectoryInfo7AddressEPv"
.LASF25:
	.string	"pmKernel"
.LASF15:
	.string	"_ZN15CPhysicalMemory11AddReservedEPvm"
.LASF12:
	.string	"AddChunk"
.LASF99:
	.string	"newStart"
.LASF23:
	.string	"_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_"
.LASF35:
	.string	"User"
.LASF38:
	.string	"Accessed"
.LASF11:
	.string	"Invalid"
.LASF44:
	.string	"SPageDirectory"
.LASF80:
	.string	"PAGE_ENTRY"
.LASF40:
	.string	"PageSize"
.LASF65:
	.string	"Release"
.LASF29:
	.string	"fmWritable"
.LASF103:
	.string	"newEntry"
.LASF68:
	.string	"_ZN18SPageDirectoryInfo6SelectEv"
.LASF74:
	.string	"CopyKernel"
.LASF92:
	.string	"DropPages"
.LASF77:
	.string	"10PAGE_ENTRY"
.LASF19:
	.string	"_ZN15CPhysicalMemory9TotalUsedEv"
.LASF34:
	.string	"Writable"
.LASF18:
	.string	"_ZN15CPhysicalMemory10TotalPagesEv"
.LASF55:
	.string	"SPageDirectoryInfo"
.LASF108:
	.string	"entryUpper"
.LASF5:
	.string	"PhysicalPointer"
.LASF48:
	.string	"Dirty"
.LASF14:
	.string	"_ZN15CPhysicalMemory8AddChunkEPvm"
.LASF56:
	.string	"pages"
.LASF53:
	.string	"SPageTable"
.LASF97:
	.string	"length"
.LASF13:
	.string	"AddReserved"
.LASF119:
	.string	"mem_physical.cpp"
.LASF28:
	.string	"9MAP_FLAGS"
	.ident	"GCC: (GNU) 4.7.1 20120721 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
