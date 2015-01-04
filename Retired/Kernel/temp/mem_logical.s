	.file	"mem_logical.cpp"
	.text
.Ltext0:
	.globl	rootAddressSpace
	.bss
	.align 32
	.type	rootAddressSpace, @object
	.size	rootAddressSpace, 4108
rootAddressSpace:
	.zero	4108
	.globl	currentAddressSpace
	.align 4
	.type	currentAddressSpace, @object
	.size	currentAddressSpace, 4
currentAddressSpace:
	.zero	4
	.globl	availableMappingPages
	.align 4
	.type	availableMappingPages, @object
	.size	availableMappingPages, 4
availableMappingPages:
	.zero	4
	.text
	.type	_ZL13InvalidateTLBPv, @function
_ZL13InvalidateTLBPv:
.LFB0:
	.file 1 "mem_logical.cpp"
	.loc 1 18 0
	.cfi_startproc
	pushl	%ebp
.LCFI0:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI1:
	.cfi_def_cfa_register 5
	.loc 1 23 0
	movl	8(%ebp), %eax
#APP
# 23 "mem_logical.cpp" 1
	invlpg (%eax)
# 0 "" 2
	.loc 1 24 0
#NO_APP
	popl	%ebp
.LCFI2:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	_ZL13InvalidateTLBPv, .-_ZL13InvalidateTLBPv
	.section	.rodata
	.align 4
.LC0:
	.string	"Kernel logical 0x%.8x, physical 0x%.8x\n"
.LC1:
	.string	"4MB page found at %i\n"
	.text
	.align 2
	.globl	_ZN18SPageDirectoryInfo10InitKernelEv
	.type	_ZN18SPageDirectoryInfo10InitKernelEv, @function
_ZN18SPageDirectoryInfo10InitKernelEv:
.LFB1:
	.loc 1 27 0
	.cfi_startproc
	pushl	%ebp
.LCFI3:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI4:
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$36, %esp
	.cfi_offset 3, -12
.LBB2:
	.loc 1 28 0
	movl	$virt, %edx
	movl	$phys, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 30 0
	movl	8(%ebp), %eax
	movl	%eax, currentAddressSpace
	.loc 1 31 0
	movl	8(%ebp), %eax
	movl	$BootPageDirectory, 4096(%eax)
	.loc 1 32 0
	movl	$BootPageDirectory, %eax
	movl	$phys, %ecx
	movl	$virt, %edx
	movl	%ecx, %ebx
	subl	%edx, %ebx
	movl	%ebx, %edx
	addl	%edx, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%edx, 4100(%eax)
	.loc 1 34 0
	movl	8(%ebp), %eax
	movl	$1, 4104(%eax)
	.loc 1 37 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movzbl	(%eax), %edx
	andl	$-2, %edx
	movb	%dl, (%eax)
	.loc 1 38 0
	movl	$0, (%esp)
	call	_ZL13InvalidateTLBPv
.LBB3:
	.loc 1 41 0
	movl	$0, -12(%ebp)
	jmp	.L3
.L6:
	.loc 1 43 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L4
	.loc 1 46 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$-128, %eax
	testb	%al, %al
	je	.L5
	.loc 1 49 0
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	$-1, (%eax,%edx,4)
	.loc 1 50 0
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC1, (%esp)
	call	_Z7kprintfPKcz
	jmp	.L5
.L4:
	.loc 1 60 0
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	$0, (%eax,%edx,4)
.L5:
	.loc 1 41 0
	addl	$1, -12(%ebp)
.L3:
	.loc 1 41 0 is_stmt 0 discriminator 1
	cmpl	$1023, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L6
.LBE3:
	.loc 1 65 0 is_stmt 1
	movl	$PageMapBootstrap, %eax
	movl	$phys, %ecx
	movl	$virt, %edx
	movl	%ecx, %ebx
	subl	%edx, %ebx
	movl	%ebx, %edx
	addl	%edx, %eax
	movl	%eax, -20(%ebp)
	.loc 1 66 0
	movl	$1023, availableMappingPages
.LBB4:
	.loc 1 67 0
	movl	$0, -16(%ebp)
	jmp	.L7
.L8:
	.loc 1 68 0 discriminator 2
	movl	-16(%ebp), %eax
	movl	$0, PageMapBootstrap(,%eax,4)
	.loc 1 67 0 discriminator 2
	addl	$1, -16(%ebp)
.L7:
	.loc 1 67 0 is_stmt 0 discriminator 1
	cmpl	$1023, -16(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L8
.LBE4:
	.loc 1 69 0 is_stmt 1
	movl	-20(%ebp), %eax
	shrl	$12, %eax
	andl	$1048575, %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	PageMapBootstrap+4092, %eax
	andl	$4095, %eax
	orl	%edx, %eax
	movl	%eax, PageMapBootstrap+4092
	.loc 1 70 0
	movzbl	PageMapBootstrap+4092, %eax
	orl	$2, %eax
	movb	%al, PageMapBootstrap+4092
	.loc 1 71 0
	movzbl	PageMapBootstrap+4092, %eax
	orl	$1, %eax
	movb	%al, PageMapBootstrap+4092
	.loc 1 73 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	addl	$4092, %eax
	movl	-20(%ebp), %edx
	shrl	$12, %edx
	andl	$1048575, %edx
	movl	%edx, %ecx
	sall	$12, %ecx
	movl	(%eax), %edx
	andl	$4095, %edx
	orl	%ecx, %edx
	movl	%edx, (%eax)
	.loc 1 74 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	addl	$4092, %eax
	movzbl	(%eax), %edx
	orl	$64, %edx
	movb	%dl, (%eax)
	.loc 1 75 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	addl	$4092, %eax
	movzbl	(%eax), %edx
	orl	$2, %edx
	movb	%dl, (%eax)
	.loc 1 76 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	addl	$4092, %eax
	movzbl	(%eax), %edx
	orl	$1, %edx
	movb	%dl, (%eax)
	.loc 1 77 0
	movl	8(%ebp), %eax
	movl	$-4096, 4092(%eax)
	.loc 1 78 0
	movl	8(%ebp), %eax
	movl	4092(%eax), %eax
	movl	%eax, (%esp)
	call	_ZL13InvalidateTLBPv
.LBE2:
	.loc 1 79 0
	addl	$36, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI5:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_ZN18SPageDirectoryInfo10InitKernelEv, .-_ZN18SPageDirectoryInfo10InitKernelEv
	.align 2
	.globl	_ZN18SPageDirectoryInfo4InitEv
	.type	_ZN18SPageDirectoryInfo4InitEv, @function
_ZN18SPageDirectoryInfo4InitEv:
.LFB2:
	.loc 1 82 0
	.cfi_startproc
	pushl	%ebp
.LCFI6:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI7:
	.cfi_def_cfa_register 5
	subl	$56, %esp
.LBB5:
	.loc 1 84 0
	movl	_ZN15CPhysicalMemory7MaximumE, %edx
	movl	_ZN15CPhysicalMemory7MinimumE, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$1, (%esp)
	call	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	movl	8(%ebp), %edx
	movl	%eax, 4100(%edx)
	.loc 1 85 0
	movl	8(%ebp), %eax
	movl	4100(%eax), %eax
	movl	$1, 16(%esp)
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	movl	8(%ebp), %edx
	movl	%eax, 4096(%edx)
.LBB6:
	.loc 1 88 0
	movl	$0, -12(%ebp)
	jmp	.L10
.L11:
	.loc 1 90 0 discriminator 2
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	$0, (%eax,%edx,4)
	.loc 1 91 0 discriminator 2
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	$0, (%eax)
	.loc 1 88 0 discriminator 2
	addl	$1, -12(%ebp)
.L10:
	.loc 1 88 0 is_stmt 0 discriminator 1
	cmpl	$1023, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L11
.LBE6:
	.loc 1 95 0 is_stmt 1
	movl	8(%ebp), %eax
	movl	$0, 4104(%eax)
.LBE5:
	.loc 1 96 0
	leave
.LCFI8:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	_ZN18SPageDirectoryInfo4InitEv, .-_ZN18SPageDirectoryInfo4InitEv
	.align 2
	.globl	_ZN18SPageDirectoryInfo7ReleaseEv
	.type	_ZN18SPageDirectoryInfo7ReleaseEv, @function
_ZN18SPageDirectoryInfo7ReleaseEv:
.LFB3:
	.loc 1 99 0
	.cfi_startproc
	pushl	%ebp
.LCFI9:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI10:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB7:
.LBB8:
	.loc 1 101 0
	movl	$0, -12(%ebp)
	jmp	.L13
.L21:
.LBB9:
	.loc 1 103 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L22
.L14:
	.loc 1 106 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$-128, %eax
	testb	%al, %al
	jne	.L23
.L16:
.LBB10:
	.loc 1 112 0
	movl	$0, -16(%ebp)
	jmp	.L17
.L20:
	.loc 1 114 0
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-16(%ebp), %edx
	movzbl	(%eax,%edx,4), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L24
.L18:
	.loc 1 116 0
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	shrl	$12, %eax
	sall	$12, %eax
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory12ReleasePagesEPvm
	jmp	.L19
.L24:
	.loc 1 115 0
	nop
.L19:
	.loc 1 112 0
	addl	$1, -16(%ebp)
.L17:
	.loc 1 112 0 is_stmt 0 discriminator 1
	cmpl	$1023, -16(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L20
	jmp	.L15
.L22:
.LBE10:
	.loc 1 104 0 is_stmt 1
	nop
	jmp	.L15
.L23:
	.loc 1 109 0
	nop
.L15:
.LBE9:
	.loc 1 101 0
	addl	$1, -12(%ebp)
.L13:
	.loc 1 101 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %eax
	movl	$phys, %edx
	shrl	$22, %edx
	cmpl	%edx, %eax
	setb	%al
	testb	%al, %al
	jne	.L21
.LBE8:
	.loc 1 121 0 is_stmt 1
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo5UnmapEPv
	.loc 1 122 0
	movl	8(%ebp), %eax
	movl	4100(%eax), %eax
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory12ReleasePagesEPvm
.LBE7:
	.loc 1 123 0
	leave
.LCFI11:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	_ZN18SPageDirectoryInfo7ReleaseEv, .-_ZN18SPageDirectoryInfo7ReleaseEv
	.align 2
	.globl	_ZN18SPageDirectoryInfo6SelectEv
	.type	_ZN18SPageDirectoryInfo6SelectEv, @function
_ZN18SPageDirectoryInfo6SelectEv:
.LFB4:
	.loc 1 126 0
	.cfi_startproc
	pushl	%ebp
.LCFI12:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI13:
	.cfi_def_cfa_register 5
	subl	$24, %esp
	.loc 1 127 0
	cmpl	$rootAddressSpace, 8(%ebp)
	je	.L26
	.loc 1 129 0
	movl	8(%ebp), %eax
	movl	4104(%eax), %edx
	movl	rootAddressSpace+4104, %eax
	cmpl	%eax, %edx
	je	.L26
	.loc 1 131 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo10CopyKernelEv
.L26:
	.loc 1 134 0
	movl	8(%ebp), %eax
	movl	4100(%eax), %eax
#APP
# 134 "mem_logical.cpp" 1
	mov %eax, %cr3
# 0 "" 2
	.loc 1 135 0
#NO_APP
	movl	8(%ebp), %eax
	movl	%eax, currentAddressSpace
	.loc 1 136 0
	leave
.LCFI14:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	_ZN18SPageDirectoryInfo6SelectEv, .-_ZN18SPageDirectoryInfo6SelectEv
	.section	.rodata
	.align 4
.LC2:
	.string	"SPageDirectoryInfo::Map(%.4x, %.2x, 0x%.8x, %i)\n"
.LC3:
	.string	"down"
.LC4:
	.string	"up"
	.align 4
.LC5:
	.string	"Searching %s between 0x%.8x and 0x%.8x (kernel base 0x%.8x)\n"
	.align 4
.LC6:
	.string	"Generated new mapping at 0x%.8x\n"
	.text
	.align 2
	.globl	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	.type	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj, @function
_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj:
.LFB5:
	.loc 1 139 0
	.cfi_startproc
	pushl	%ebp
.LCFI15:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI16:
	.cfi_def_cfa_register 5
	subl	$88, %esp
.LBB11:
	.loc 1 140 0
	movl	24(%ebp), %ecx
	movl	20(%ebp), %edx
	movl	16(%ebp), %eax
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 148 0
	cmpl	$1, 16(%ebp)
	jne	.L28
	.loc 1 148 0 is_stmt 0 discriminator 1
	cmpl	$rootAddressSpace, 8(%ebp)
	je	.L28
	.loc 1 150 0 is_stmt 1
	movl	24(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$rootAddressSpace, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	movl	%eax, -24(%ebp)
	.loc 1 151 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo10CopyKernelEv
	.loc 1 152 0
	movl	-24(%ebp), %eax
	jmp	.L29
.L28:
	.loc 1 156 0
	movb	$0, -17(%ebp)
	.loc 1 157 0
	movl	16(%ebp), %eax
	testl	%eax, %eax
	je	.L31
	cmpl	$1, %eax
	je	.L32
	jmp	.L61
.L31:
	.loc 1 160 0
	movl	12(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L33
	.loc 1 161 0
	movl	$0, %eax
	jmp	.L29
.L33:
	.loc 1 162 0
	movl	$4096, -12(%ebp)
	.loc 1 163 0
	movl	$virt, %eax
	subl	$4096, %eax
	movl	%eax, -16(%ebp)
	.loc 1 164 0
	jmp	.L34
.L32:
	.loc 1 166 0
	movl	12(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L35
	.loc 1 168 0
	cmpl	$1, 24(%ebp)
	je	.L36
	.loc 1 169 0
	movl	$0, %eax
	jmp	.L29
.L36:
	.loc 1 170 0
	movb	$1, -17(%ebp)
.L35:
	.loc 1 173 0
	movl	$virt, -12(%ebp)
	.loc 1 174 0
	movl	$-4096, -16(%ebp)
	.loc 1 175 0
	jmp	.L34
.L61:
	.loc 1 177 0
	movl	$0, %eax
	jmp	.L29
.L34:
	.loc 1 184 0
	movl	-16(%ebp), %ecx
	movl	-12(%ebp), %edx
	cmpb	$0, -17(%ebp)
	je	.L37
	.loc 1 184 0 is_stmt 0 discriminator 1
	movl	$.LC3, %eax
	jmp	.L38
.L37:
	.loc 1 184 0 discriminator 2
	movl	$.LC4, %eax
.L38:
	.loc 1 184 0 discriminator 3
	movl	$virt, 16(%esp)
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC5, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 185 0 is_stmt 1 discriminator 3
	movl	$0, -24(%ebp)
	.loc 1 186 0 discriminator 3
	cmpb	$0, -17(%ebp)
	je	.L39
	.loc 1 186 0 is_stmt 0 discriminator 1
	movl	-16(%ebp), %eax
	jmp	.L40
.L39:
	.loc 1 186 0 discriminator 2
	movl	-12(%ebp), %eax
.L40:
	.loc 1 186 0 discriminator 3
	movl	%eax, -28(%ebp)
	.loc 1 187 0 is_stmt 1 discriminator 3
	movl	$0, -32(%ebp)
	.loc 1 188 0 discriminator 3
	jmp	.L41
.L48:
.LBB12:
	.loc 1 190 0
	movl	-12(%ebp), %eax
	cmpl	-28(%ebp), %eax
	ja	.L42
	.loc 1 190 0 is_stmt 0 discriminator 1
	movl	-16(%ebp), %eax
	cmpl	-28(%ebp), %eax
	jae	.L43
.L42:
	.loc 1 191 0 is_stmt 1
	movl	$0, %eax
	jmp	.L29
.L43:
	.loc 1 193 0
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo7AddressEPv
	movl	%eax, -44(%ebp)
	.loc 1 194 0
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo7AddressEPv
	movl	_ZN15CPhysicalMemory7InvalidE, %edx
	cmpl	%edx, %eax
	sete	%al
	testb	%al, %al
	je	.L44
	.loc 1 196 0
	cmpl	$0, -32(%ebp)
	jne	.L45
	.loc 1 197 0
	movl	-28(%ebp), %eax
	movl	%eax, -24(%ebp)
.L45:
	.loc 1 198 0
	addl	$1, -32(%ebp)
	jmp	.L46
.L44:
	.loc 1 202 0
	movl	$0, -32(%ebp)
.L46:
	.loc 1 205 0
	cmpb	$0, -17(%ebp)
	je	.L47
	.loc 1 206 0
	movl	-28(%ebp), %eax
	subl	$4096, %eax
	movl	%eax, -28(%ebp)
	jmp	.L41
.L47:
	.loc 1 208 0
	movl	-28(%ebp), %eax
	addl	$4096, %eax
	movl	%eax, -28(%ebp)
.L41:
.LBE12:
	.loc 1 188 0 discriminator 1
	movl	-32(%ebp), %eax
	cmpl	24(%ebp), %eax
	setb	%al
	testb	%al, %al
	jne	.L48
	.loc 1 210 0
	cmpl	$0, -24(%ebp)
	jne	.L49
	.loc 1 211 0
	movl	$0, %eax
	jmp	.L29
.L49:
.LBB13:
	.loc 1 213 0
	movl	12(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L50
	.loc 1 213 0 is_stmt 0 discriminator 1
	movl	availableMappingPages, %eax
	cmpl	$1, %eax
	jne	.L50
.LBB14:
	.loc 1 218 0 is_stmt 1
	movl	$1023, -36(%ebp)
	jmp	.L51
.L54:
	.loc 1 220 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-36(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L62
.L52:
	.loc 1 218 0
	subl	$1, -36(%ebp)
.L51:
	.loc 1 218 0 is_stmt 0 discriminator 1
	movl	-36(%ebp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	jne	.L54
	jmp	.L53
.L62:
	.loc 1 221 0 is_stmt 1
	nop
.L53:
	.loc 1 223 0
	cmpl	$0, -36(%ebp)
	jns	.L55
	.loc 1 224 0
	movl	$0, %eax
	jmp	.L29
.L55:
	.loc 1 227 0
	movl	_ZN15CPhysicalMemory7MaximumE, %edx
	movl	_ZN15CPhysicalMemory7MinimumE, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$1, (%esp)
	call	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	movl	%eax, -48(%ebp)
	.loc 1 228 0
	movl	-48(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$1, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEiPvS0_
	.loc 1 229 0
	movl	-24(%ebp), %eax
	movl	%eax, -52(%ebp)
.LBB15:
	.loc 1 232 0
	movl	$0, -40(%ebp)
	jmp	.L56
.L57:
	.loc 1 233 0 discriminator 2
	movl	-52(%ebp), %eax
	movl	-40(%ebp), %edx
	movl	$0, (%eax,%edx,4)
	.loc 1 232 0 discriminator 2
	addl	$1, -40(%ebp)
.L56:
	.loc 1 232 0 is_stmt 0 discriminator 1
	cmpl	$1023, -40(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L57
.LBE15:
	.loc 1 236 0 is_stmt 1
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-36(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	-48(%ebp), %edx
	shrl	$12, %edx
	andl	$1048575, %edx
	movl	%edx, %ecx
	sall	$12, %ecx
	movl	(%eax), %edx
	andl	$4095, %edx
	orl	%ecx, %edx
	movl	%edx, (%eax)
	.loc 1 237 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-36(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %edx
	orl	$2, %edx
	movb	%dl, (%eax)
	.loc 1 238 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-36(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %edx
	orl	$64, %edx
	movb	%dl, (%eax)
	.loc 1 239 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-36(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %edx
	orl	$1, %edx
	movb	%dl, (%eax)
	.loc 1 240 0
	movl	8(%ebp), %eax
	movl	-36(%ebp), %edx
	movl	-52(%ebp), %ecx
	movl	%ecx, (%eax,%edx,4)
	.loc 1 243 0
	movl	availableMappingPages, %eax
	addl	$1023, %eax
	movl	%eax, availableMappingPages
	.loc 1 246 0
	movl	-36(%ebp), %eax
	sall	$22, %eax
	orl	$16773120, %eax
	movl	%eax, -24(%ebp)
.L50:
.LBE14:
.LBE13:
	.loc 1 250 0
	movl	$0, -32(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, -28(%ebp)
	jmp	.L58
.L59:
	.loc 1 253 0 discriminator 2
	movl	20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEiPvS0_
	.loc 1 250 0 discriminator 2
	addl	$1, -32(%ebp)
	addl	$4096, -28(%ebp)
	addl	$4096, 20(%ebp)
.L58:
	.loc 1 250 0 is_stmt 0 discriminator 1
	movl	-32(%ebp), %eax
	cmpl	24(%ebp), %eax
	setb	%al
	testb	%al, %al
	jne	.L59
	.loc 1 256 0 is_stmt 1
	cmpl	$rootAddressSpace, 8(%ebp)
	jne	.L60
	.loc 1 258 0
	movl	8(%ebp), %eax
	movl	4104(%eax), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	movl	%edx, 4104(%eax)
	.loc 1 259 0
	movl	8(%ebp), %eax
	movl	4104(%eax), %eax
	testl	%eax, %eax
	jne	.L60
	.loc 1 260 0
	movl	8(%ebp), %eax
	movl	$1, 4104(%eax)
.L60:
	.loc 1 264 0
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC6, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 265 0
	movl	-24(%ebp), %eax
.L29:
.LBE11:
	.loc 1 266 0
	leave
.LCFI17:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj, .-_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	.section	.rodata
	.align 4
.LC7:
	.string	"SPageDirectoryInfo::Map(%.4x, 0x%.8x, 0x%.8x)\n"
.LC8:
	.string	"0x%.8x mapped\n"
	.text
	.align 2
	.globl	_ZN18SPageDirectoryInfo3MapEiPvS0_
	.type	_ZN18SPageDirectoryInfo3MapEiPvS0_, @function
_ZN18SPageDirectoryInfo3MapEiPvS0_:
.LFB6:
	.loc 1 269 0
	.cfi_startproc
	pushl	%ebp
.LCFI18:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI19:
	.cfi_def_cfa_register 5
	subl	$72, %esp
.LBB16:
	.loc 1 270 0
	movl	20(%ebp), %edx
	movl	16(%ebp), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC7, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 271 0
	movl	$48, (%esp)
	call	_Z4testc
	.loc 1 273 0
	movl	16(%ebp), %eax
	shrl	$22, %eax
	movl	%eax, -16(%ebp)
	.loc 1 274 0
	movl	16(%ebp), %eax
	shrl	$12, %eax
	andl	$1023, %eax
	movl	%eax, -20(%ebp)
	.loc 1 275 0
	movl	16(%ebp), %eax
	andl	$4095, %eax
	movl	%eax, -24(%ebp)
.LBB17:
	.loc 1 278 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	jne	.L64
.LBB18:
	.loc 1 280 0
	movl	$49, (%esp)
	call	_Z4testc
	.loc 1 281 0
	movl	_ZN15CPhysicalMemory7MaximumE, %edx
	movl	_ZN15CPhysicalMemory7MinimumE, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$1, (%esp)
	call	_ZN15CPhysicalMemory23AllocateContiguousPagesEmPvS0_
	movl	%eax, -28(%ebp)
	.loc 1 282 0
	movl	$50, (%esp)
	call	_Z4testc
	.loc 1 283 0
	movl	$1, 16(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	$5, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	movl	8(%ebp), %edx
	movl	-16(%ebp), %ecx
	movl	%eax, (%edx,%ecx,4)
	.loc 1 285 0
	movl	$51, (%esp)
	call	_Z4testc
.LBB19:
	.loc 1 287 0
	movl	$0, -12(%ebp)
	jmp	.L65
.L66:
	.loc 1 288 0 discriminator 2
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-12(%ebp), %edx
	movl	$0, (%eax,%edx,4)
	.loc 1 287 0 discriminator 2
	addl	$1, -12(%ebp)
.L65:
	.loc 1 287 0 is_stmt 0 discriminator 1
	cmpl	$1023, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L66
.LBE19:
	.loc 1 291 0 is_stmt 1
	movl	$52, (%esp)
	call	_Z4testc
	.loc 1 294 0
	movl	$0, -36(%ebp)
	.loc 1 295 0
	movl	-28(%ebp), %eax
	shrl	$12, %eax
	andl	$1048575, %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	andl	$4095, %eax
	orl	%edx, %eax
	movl	%eax, -36(%ebp)
	.loc 1 296 0
	movzbl	-36(%ebp), %eax
	orl	$1, %eax
	movb	%al, -36(%ebp)
	.loc 1 297 0
	movzbl	-36(%ebp), %eax
	orl	$2, %eax
	movb	%al, -36(%ebp)
	.loc 1 298 0
	movzbl	-36(%ebp), %eax
	andl	$-5, %eax
	movb	%al, -36(%ebp)
	.loc 1 299 0
	movzbl	-36(%ebp), %eax
	andl	$-9, %eax
	movb	%al, -36(%ebp)
	.loc 1 300 0
	movzbl	-36(%ebp), %eax
	andl	$-17, %eax
	movb	%al, -36(%ebp)
	.loc 1 301 0
	movzbl	-36(%ebp), %eax
	andl	$-33, %eax
	movb	%al, -36(%ebp)
	.loc 1 302 0
	movzbl	-36(%ebp), %eax
	orl	$64, %eax
	movb	%al, -36(%ebp)
	.loc 1 303 0
	movzbl	-36(%ebp), %eax
	andl	$127, %eax
	movb	%al, -36(%ebp)
	.loc 1 304 0
	movzbl	-35(%ebp), %eax
	andl	$-2, %eax
	movb	%al, -35(%ebp)
	.loc 1 305 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%eax, %edx
	movl	-36(%ebp), %eax
	movl	%eax, (%edx)
.L64:
.LBE18:
.LBE17:
	.loc 1 308 0
	movl	$53, (%esp)
	call	_Z4testc
	.loc 1 310 0
	movl	$0, -32(%ebp)
	.loc 1 311 0
	movzbl	-32(%ebp), %eax
	orl	$1, %eax
	movb	%al, -32(%ebp)
	.loc 1 312 0
	movl	12(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L67
	.loc 1 313 0
	movzbl	-32(%ebp), %eax
	orl	$2, %eax
	movb	%al, -32(%ebp)
.L67:
	.loc 1 314 0
	movl	12(%ebp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	.L68
	.loc 1 315 0
	movzbl	-32(%ebp), %eax
	orl	$4, %eax
	movb	%al, -32(%ebp)
.L68:
	.loc 1 316 0
	movl	20(%ebp), %eax
	shrl	$12, %eax
	andl	$1048575, %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-32(%ebp), %eax
	andl	$4095, %eax
	orl	%edx, %eax
	movl	%eax, -32(%ebp)
	.loc 1 319 0
	movl	$54, (%esp)
	call	_Z4testc
	.loc 1 320 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-20(%ebp), %edx
	movl	-32(%ebp), %ecx
	movl	%ecx, (%eax,%edx,4)
	.loc 1 321 0
	movl	$55, (%esp)
	call	_Z4testc
	.loc 1 322 0
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZL13InvalidateTLBPv
	.loc 1 323 0
	movl	16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC8, (%esp)
	call	_Z7kprintfPKcz
.LBE16:
	.loc 1 324 0
	leave
.LCFI20:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	_ZN18SPageDirectoryInfo3MapEiPvS0_, .-_ZN18SPageDirectoryInfo3MapEiPvS0_
	.align 2
	.globl	_ZN18SPageDirectoryInfo5UnmapEPv
	.type	_ZN18SPageDirectoryInfo5UnmapEPv, @function
_ZN18SPageDirectoryInfo5UnmapEPv:
.LFB7:
	.loc 1 327 0
	.cfi_startproc
	pushl	%ebp
.LCFI21:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI22:
	.cfi_def_cfa_register 5
	subl	$24, %esp
	.loc 1 328 0
	movl	$0, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo5UnmapEPvb
	.loc 1 329 0
	leave
.LCFI23:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	_ZN18SPageDirectoryInfo5UnmapEPv, .-_ZN18SPageDirectoryInfo5UnmapEPv
	.align 2
	.globl	_ZN18SPageDirectoryInfo5UnmapEPvb
	.type	_ZN18SPageDirectoryInfo5UnmapEPvb, @function
_ZN18SPageDirectoryInfo5UnmapEPvb:
.LFB8:
	.loc 1 332 0
	.cfi_startproc
	pushl	%ebp
.LCFI24:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI25:
	.cfi_def_cfa_register 5
	subl	$56, %esp
	movl	16(%ebp), %eax
	movb	%al, -28(%ebp)
.LBB20:
	.loc 1 334 0
	cmpl	$PageMapBootstrap, 12(%ebp)
	je	.L80
.L71:
	.loc 1 338 0
	movl	12(%ebp), %eax
	shrl	$22, %eax
	movl	%eax, -16(%ebp)
	.loc 1 339 0
	movl	12(%ebp), %eax
	shrl	$12, %eax
	andl	$1023, %eax
	movl	%eax, -20(%ebp)
	.loc 1 340 0
	movl	12(%ebp), %eax
	andl	$4095, %eax
	movl	%eax, -24(%ebp)
	.loc 1 343 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L81
.L73:
	.loc 1 345 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	cmpl	$-1, %eax
	je	.L82
.L74:
	.loc 1 347 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-20(%ebp), %edx
	movzbl	(%eax,%edx,4), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L83
.L75:
	.loc 1 351 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-20(%ebp), %edx
	movzbl	(%eax,%edx,4), %ecx
	andl	$-2, %ecx
	movb	%cl, (%eax,%edx,4)
	.loc 1 352 0
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZL13InvalidateTLBPv
	.loc 1 353 0
	cmpb	$0, -28(%ebp)
	je	.L76
	.loc 1 354 0
	movl	availableMappingPages, %eax
	addl	$1, %eax
	movl	%eax, availableMappingPages
.L76:
.LBB21:
	.loc 1 357 0
	movl	$0, -12(%ebp)
	jmp	.L77
.L79:
	.loc 1 359 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	-12(%ebp), %edx
	movzbl	(%eax,%edx,4), %eax
	andl	$1, %eax
	testb	%al, %al
	jne	.L84
.L78:
	.loc 1 357 0
	addl	$1, -12(%ebp)
.L77:
	.loc 1 357 0 is_stmt 0 discriminator 1
	cmpl	$1023, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L79
.LBE21:
	.loc 1 364 0 is_stmt 1
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %edx
	andl	$-2, %edx
	movb	%dl, (%eax)
	.loc 1 365 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	$1, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18SPageDirectoryInfo5UnmapEPvb
	.loc 1 366 0
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	$0, (%eax,%edx,4)
	.loc 1 367 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	shrl	$12, %eax
	sall	$12, %eax
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory12ReleasePagesEPvm
	.loc 1 368 0
	cmpb	$0, -28(%ebp)
	je	.L70
	.loc 1 369 0
	movl	availableMappingPages, %eax
	subl	$1023, %eax
	movl	%eax, availableMappingPages
	jmp	.L70
.L80:
	.loc 1 335 0
	nop
	jmp	.L70
.L81:
	.loc 1 344 0
	nop
	jmp	.L70
.L82:
	.loc 1 346 0
	nop
	jmp	.L70
.L83:
	.loc 1 348 0
	nop
	jmp	.L70
.L84:
.LBB22:
	.loc 1 360 0
	nop
.L70:
.LBE22:
.LBE20:
	.loc 1 370 0
	leave
.LCFI26:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	_ZN18SPageDirectoryInfo5UnmapEPvb, .-_ZN18SPageDirectoryInfo5UnmapEPvb
	.align 2
	.globl	_ZN18SPageDirectoryInfo7AddressEPv
	.type	_ZN18SPageDirectoryInfo7AddressEPv, @function
_ZN18SPageDirectoryInfo7AddressEPv:
.LFB9:
	.loc 1 373 0
	.cfi_startproc
	pushl	%ebp
.LCFI27:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI28:
	.cfi_def_cfa_register 5
	subl	$16, %esp
.LBB23:
	.loc 1 376 0
	movl	12(%ebp), %eax
	shrl	$22, %eax
	movl	%eax, -4(%ebp)
	.loc 1 377 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-4(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	jne	.L86
	.loc 1 378 0
	movl	_ZN15CPhysicalMemory7InvalidE, %eax
	jmp	.L87
.L86:
	.loc 1 382 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-4(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	andl	$-128, %eax
	testb	%al, %al
	je	.L88
	.loc 1 383 0
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-4(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	shrl	$12, %eax
	sall	$12, %eax
	movl	%eax, %edx
	andl	$-4194304, %edx
	movl	12(%ebp), %eax
	andl	$4194303, %eax
	orl	%edx, %eax
	jmp	.L87
.L88:
	.loc 1 387 0
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	%eax, -8(%ebp)
	.loc 1 388 0
	movl	12(%ebp), %eax
	andl	$4190208, %eax
	shrl	$12, %eax
	movl	%eax, -12(%ebp)
	.loc 1 389 0
	movl	-8(%ebp), %eax
	movl	-12(%ebp), %edx
	movzbl	(%eax,%edx,4), %eax
	andl	$1, %eax
	testb	%al, %al
	jne	.L89
	.loc 1 390 0
	movl	_ZN15CPhysicalMemory7InvalidE, %eax
	jmp	.L87
.L89:
	.loc 1 392 0
	movl	-8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	shrl	$12, %eax
	sall	$12, %eax
	movl	12(%ebp), %edx
	andl	$4095, %edx
	orl	%edx, %eax
.L87:
.LBE23:
	.loc 1 393 0
	leave
.LCFI29:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	_ZN18SPageDirectoryInfo7AddressEPv, .-_ZN18SPageDirectoryInfo7AddressEPv
	.align 2
	.globl	_ZN18SPageDirectoryInfo10CopyKernelEv
	.type	_ZN18SPageDirectoryInfo10CopyKernelEv, @function
_ZN18SPageDirectoryInfo10CopyKernelEv:
.LFB10:
	.loc 1 396 0
	.cfi_startproc
	pushl	%ebp
.LCFI30:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI31:
	.cfi_def_cfa_register 5
	subl	$16, %esp
.LBB24:
.LBB25:
	.loc 1 397 0
	movl	$virt, %eax
	shrl	$22, %eax
	movl	%eax, -4(%ebp)
	jmp	.L91
.L92:
	.loc 1 399 0 discriminator 2
	movl	-4(%ebp), %eax
	movl	rootAddressSpace(,%eax,4), %ecx
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%ecx, (%eax,%edx,4)
	.loc 1 400 0 discriminator 2
	movl	8(%ebp), %eax
	movl	4096(%eax), %eax
	movl	-4(%ebp), %edx
	sall	$2, %edx
	addl	%eax, %edx
	movl	rootAddressSpace+4096, %eax
	movl	-4(%ebp), %ecx
	sall	$2, %ecx
	addl	%ecx, %eax
	movl	(%eax), %eax
	movl	%eax, (%edx)
	.loc 1 397 0 discriminator 2
	addl	$1, -4(%ebp)
.L91:
	.loc 1 397 0 is_stmt 0 discriminator 1
	cmpl	$1023, -4(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L92
.LBE25:
	.loc 1 402 0 is_stmt 1
	movl	rootAddressSpace+4104, %edx
	movl	8(%ebp), %eax
	movl	%edx, 4104(%eax)
.LBE24:
	.loc 1 403 0
	leave
.LCFI32:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.size	_ZN18SPageDirectoryInfo10CopyKernelEv, .-_ZN18SPageDirectoryInfo10CopyKernelEv
.Letext0:
	.file 2 "runtime.h"
	.file 3 "mem_physical.h"
	.file 4 "mem_logical.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0xa01
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF87
	.byte	0x4
	.long	.LASF88
	.long	.LASF89
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
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF6
	.uleb128 0x5
	.byte	0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF10
	.long	0x7d
	.uleb128 0x6
	.long	.LASF7
	.sleb128 0
	.uleb128 0x6
	.long	.LASF8
	.sleb128 1
	.byte	0
	.uleb128 0x2
	.long	.LASF9
	.byte	0x4
	.byte	0xa
	.long	0x64
	.uleb128 0x5
	.byte	0x4
	.byte	0x4
	.byte	0xc
	.long	.LASF11
	.long	0xa7
	.uleb128 0x6
	.long	.LASF12
	.sleb128 1
	.uleb128 0x6
	.long	.LASF13
	.sleb128 2
	.uleb128 0x6
	.long	.LASF14
	.sleb128 4
	.byte	0
	.uleb128 0x2
	.long	.LASF15
	.byte	0x4
	.byte	0x10
	.long	0x88
	.uleb128 0x7
	.byte	0x4
	.byte	0x4
	.byte	0x12
	.long	.LASF28
	.long	0x1a4
	.uleb128 0x8
	.byte	0x4
	.byte	0x4
	.byte	0x13
	.long	0x19b
	.uleb128 0x9
	.byte	0x4
	.byte	0x4
	.byte	0x14
	.long	0x18a
	.uleb128 0xa
	.long	.LASF16
	.byte	0x4
	.byte	0x15
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x4
	.byte	0x16
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF18
	.byte	0x4
	.byte	0x17
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF19
	.byte	0x4
	.byte	0x18
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF20
	.byte	0x4
	.byte	0x19
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF21
	.byte	0x4
	.byte	0x1a
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF22
	.byte	0x4
	.byte	0x1b
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF23
	.byte	0x4
	.byte	0x1c
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF24
	.byte	0x4
	.byte	0x1d
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF25
	.byte	0x4
	.byte	0x1e
	.long	0x45
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF26
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
	.uleb128 0xb
	.long	0xc6
	.uleb128 0xc
	.long	.LASF33
	.byte	0x4
	.byte	0x21
	.long	0x45
	.byte	0
	.uleb128 0xd
	.long	0xbe
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF27
	.byte	0x4
	.byte	0x23
	.long	0xb2
	.uleb128 0x7
	.byte	0x4
	.byte	0x4
	.byte	0x25
	.long	.LASF29
	.long	0x2a1
	.uleb128 0x8
	.byte	0x4
	.byte	0x4
	.byte	0x26
	.long	0x298
	.uleb128 0x9
	.byte	0x4
	.byte	0x4
	.byte	0x27
	.long	0x287
	.uleb128 0xa
	.long	.LASF16
	.byte	0x4
	.byte	0x28
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x4
	.byte	0x29
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF18
	.byte	0x4
	.byte	0x2a
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF30
	.byte	0x4
	.byte	0x2b
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF20
	.byte	0x4
	.byte	0x2c
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF21
	.byte	0x4
	.byte	0x2d
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF31
	.byte	0x4
	.byte	0x2e
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF32
	.byte	0x4
	.byte	0x2f
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF24
	.byte	0x4
	.byte	0x30
	.long	0x45
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF25
	.byte	0x4
	.byte	0x31
	.long	0x45
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.long	.LASF26
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
	.uleb128 0xb
	.long	0x1c3
	.uleb128 0xc
	.long	.LASF33
	.byte	0x4
	.byte	0x34
	.long	0x45
	.byte	0
	.uleb128 0xd
	.long	0x1bb
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF34
	.byte	0x4
	.byte	0x36
	.long	0x1af
	.uleb128 0xe
	.value	0x1000
	.byte	0x4
	.byte	0x38
	.long	.LASF90
	.long	0x2c8
	.uleb128 0xf
	.long	.LASF37
	.byte	0x4
	.byte	0x39
	.long	0x2c8
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x10
	.long	0x2a1
	.long	0x2d9
	.uleb128 0x11
	.long	0x2d9
	.value	0x3ff
	.byte	0
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF35
	.uleb128 0x2
	.long	.LASF36
	.byte	0x4
	.byte	0x3a
	.long	0x2ac
	.uleb128 0x12
	.long	.LASF91
	.value	0x100c
	.byte	0x4
	.byte	0x3c
	.long	0x48a
	.uleb128 0xf
	.long	.LASF38
	.byte	0x4
	.byte	0x40
	.long	0x48a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xf
	.long	.LASF39
	.byte	0x4
	.byte	0x41
	.long	0x4a1
	.byte	0x3
	.byte	0x23
	.uleb128 0x1000
	.uleb128 0xf
	.long	.LASF40
	.byte	0x4
	.byte	0x42
	.long	0x50
	.byte	0x3
	.byte	0x23
	.uleb128 0x1004
	.uleb128 0x13
	.long	.LASF92
	.byte	0x4
	.byte	0x53
	.long	0x4a7
	.byte	0x3
	.byte	0x23
	.uleb128 0x1008
	.byte	0x3
	.uleb128 0x14
	.byte	0x1
	.long	.LASF41
	.byte	0x4
	.byte	0x44
	.long	.LASF43
	.byte	0x1
	.long	0x349
	.long	0x350
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.byte	0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF42
	.byte	0x4
	.byte	0x46
	.long	.LASF44
	.byte	0x1
	.long	0x365
	.long	0x36c
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.byte	0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF45
	.byte	0x4
	.byte	0x47
	.long	.LASF46
	.byte	0x1
	.long	0x381
	.long	0x388
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.byte	0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF47
	.byte	0x4
	.byte	0x48
	.long	.LASF48
	.byte	0x1
	.long	0x39d
	.long	0x3a4
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.byte	0
	.uleb128 0x16
	.byte	0x1
	.string	"Map"
	.byte	0x4
	.byte	0x4a
	.long	.LASF93
	.long	0x5b
	.byte	0x1
	.long	0x3bd
	.long	0x3d8
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.uleb128 0x17
	.long	0x4a7
	.uleb128 0x17
	.long	0x7d
	.uleb128 0x17
	.long	0x50
	.uleb128 0x17
	.long	0x25
	.byte	0
	.uleb128 0x18
	.byte	0x1
	.string	"Map"
	.byte	0x4
	.byte	0x4b
	.long	.LASF94
	.byte	0x1
	.long	0x3ed
	.long	0x403
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.uleb128 0x17
	.long	0x4a7
	.uleb128 0x17
	.long	0x5b
	.uleb128 0x17
	.long	0x50
	.byte	0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF49
	.byte	0x4
	.byte	0x4c
	.long	.LASF50
	.byte	0x1
	.long	0x418
	.long	0x424
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.uleb128 0x17
	.long	0x5b
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF51
	.byte	0x4
	.byte	0x4d
	.long	.LASF52
	.long	0x50
	.byte	0x1
	.long	0x43d
	.long	0x449
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.uleb128 0x17
	.long	0x5b
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF49
	.byte	0x4
	.byte	0x50
	.long	.LASF53
	.byte	0x3
	.byte	0x1
	.long	0x45f
	.long	0x470
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.uleb128 0x17
	.long	0x5b
	.uleb128 0x17
	.long	0x4b4
	.byte	0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF54
	.byte	0x4
	.byte	0x51
	.long	.LASF55
	.byte	0x3
	.byte	0x1
	.long	0x482
	.uleb128 0x15
	.long	0x4ae
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x10
	.long	0x49b
	.long	0x49b
	.uleb128 0x11
	.long	0x2d9
	.value	0x3ff
	.byte	0
	.uleb128 0x1c
	.byte	0x4
	.long	0x2e0
	.uleb128 0x1c
	.byte	0x4
	.long	0x1a4
	.uleb128 0x1d
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x1c
	.byte	0x4
	.long	0x2eb
	.uleb128 0x3
	.byte	0x1
	.byte	0x2
	.long	.LASF56
	.uleb128 0x1e
	.long	.LASF95
	.byte	0x1
	.byte	0x11
	.long	.LFB0
	.long	.LFE0
	.long	.LLST0
	.byte	0x1
	.long	0x4e0
	.uleb128 0x1f
	.string	"m"
	.byte	0x1
	.byte	0x11
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x20
	.long	0x334
	.byte	0x1
	.byte	0x1a
	.long	.LFB1
	.long	.LFE1
	.long	.LLST1
	.long	0x4fc
	.byte	0x1
	.long	0x552
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x22
	.long	.LBB2
	.long	.LBE2
	.uleb128 0x23
	.long	.LASF57
	.byte	0x1
	.byte	0x41
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x24
	.long	.LBB3
	.long	.LBE3
	.long	0x53a
	.uleb128 0x25
	.string	"i"
	.byte	0x1
	.byte	0x29
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x22
	.long	.LBB4
	.long	.LBE4
	.uleb128 0x25
	.string	"i"
	.byte	0x1
	.byte	0x43
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x26
	.long	0x4ae
	.uleb128 0x20
	.long	0x350
	.byte	0x1
	.byte	0x51
	.long	.LFB2
	.long	.LFE2
	.long	.LLST2
	.long	0x573
	.byte	0x1
	.long	0x597
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x22
	.long	.LBB6
	.long	.LBE6
	.uleb128 0x25
	.string	"i"
	.byte	0x1
	.byte	0x58
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x20
	.long	0x36c
	.byte	0x1
	.byte	0x62
	.long	.LFB3
	.long	.LFE3
	.long	.LLST3
	.long	0x5b3
	.byte	0x1
	.long	0x5ed
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x22
	.long	.LBB8
	.long	.LBE8
	.uleb128 0x25
	.string	"i"
	.byte	0x1
	.byte	0x65
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x22
	.long	.LBB10
	.long	.LBE10
	.uleb128 0x25
	.string	"j"
	.byte	0x1
	.byte	0x70
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x20
	.long	0x388
	.byte	0x1
	.byte	0x7d
	.long	.LFB4
	.long	.LFE4
	.long	.LLST4
	.long	0x609
	.byte	0x1
	.long	0x617
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x20
	.long	0x3a4
	.byte	0x1
	.byte	0x8a
	.long	.LFB5
	.long	.LFE5
	.long	.LLST5
	.long	0x633
	.byte	0x1
	.long	0x73d
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x27
	.long	.LASF59
	.byte	0x1
	.byte	0x8a
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x27
	.long	.LASF60
	.byte	0x1
	.byte	0x8a
	.long	0x7d
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x27
	.long	.LASF61
	.byte	0x1
	.byte	0x8a
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 12
	.uleb128 0x27
	.long	.LASF62
	.byte	0x1
	.byte	0x8a
	.long	0x25
	.byte	0x2
	.byte	0x91
	.sleb128 16
	.uleb128 0x22
	.long	.LBB11
	.long	.LBE11
	.uleb128 0x25
	.string	"min"
	.byte	0x1
	.byte	0x8e
	.long	0x45
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x25
	.string	"max"
	.byte	0x1
	.byte	0x8e
	.long	0x45
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x23
	.long	.LASF63
	.byte	0x1
	.byte	0x8f
	.long	0x4b4
	.byte	0x2
	.byte	0x91
	.sleb128 -25
	.uleb128 0x23
	.long	.LASF64
	.byte	0x1
	.byte	0x91
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x23
	.long	.LASF65
	.byte	0x1
	.byte	0x91
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x23
	.long	.LASF66
	.byte	0x1
	.byte	0x92
	.long	0x25
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x24
	.long	.LBB12
	.long	.LBE12
	.long	0x6f1
	.uleb128 0x25
	.string	"ptr"
	.byte	0x1
	.byte	0xc1
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -52
	.byte	0
	.uleb128 0x22
	.long	.LBB14
	.long	.LBE14
	.uleb128 0x23
	.long	.LASF67
	.byte	0x1
	.byte	0xd7
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x23
	.long	.LASF68
	.byte	0x1
	.byte	0xe3
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x23
	.long	.LASF69
	.byte	0x1
	.byte	0xe5
	.long	0x49b
	.byte	0x2
	.byte	0x91
	.sleb128 -60
	.uleb128 0x22
	.long	.LBB15
	.long	.LBE15
	.uleb128 0x25
	.string	"i"
	.byte	0x1
	.byte	0xe8
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -48
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x28
	.long	0x3d8
	.byte	0x1
	.value	0x10c
	.long	.LFB6
	.long	.LFE6
	.long	.LLST6
	.long	0x75a
	.byte	0x1
	.long	0x81a
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x29
	.long	.LASF59
	.byte	0x1
	.value	0x10c
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x29
	.long	.LASF60
	.byte	0x1
	.value	0x10c
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x29
	.long	.LASF61
	.byte	0x1
	.value	0x10c
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 12
	.uleb128 0x22
	.long	.LBB16
	.long	.LBE16
	.uleb128 0x2a
	.long	.LASF70
	.byte	0x1
	.value	0x111
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x2a
	.long	.LASF71
	.byte	0x1
	.value	0x112
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2a
	.long	.LASF72
	.byte	0x1
	.value	0x113
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2a
	.long	.LASF73
	.byte	0x1
	.value	0x135
	.long	0x2a1
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x22
	.long	.LBB18
	.long	.LBE18
	.uleb128 0x2a
	.long	.LASF68
	.byte	0x1
	.value	0x119
	.long	0x50
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x2a
	.long	.LASF74
	.byte	0x1
	.value	0x125
	.long	0x1a4
	.byte	0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x22
	.long	.LBB19
	.long	.LBE19
	.uleb128 0x2b
	.string	"i"
	.byte	0x1
	.value	0x11f
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x28
	.long	0x403
	.byte	0x1
	.value	0x146
	.long	.LFB7
	.long	.LFE7
	.long	.LLST7
	.long	0x837
	.byte	0x1
	.long	0x854
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x29
	.long	.LASF75
	.byte	0x1
	.value	0x146
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.byte	0
	.uleb128 0x28
	.long	0x449
	.byte	0x1
	.value	0x14b
	.long	.LFB8
	.long	.LFE8
	.long	.LLST8
	.long	0x871
	.byte	0x1
	.long	0x8e7
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x29
	.long	.LASF75
	.byte	0x1
	.value	0x14b
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x29
	.long	.LASF76
	.byte	0x1
	.value	0x14b
	.long	0x4b4
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x22
	.long	.LBB20
	.long	.LBE20
	.uleb128 0x2a
	.long	.LASF70
	.byte	0x1
	.value	0x152
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x2a
	.long	.LASF71
	.byte	0x1
	.value	0x153
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2a
	.long	.LASF72
	.byte	0x1
	.value	0x154
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2c
	.long	.Ldebug_ranges0+0
	.uleb128 0x2b
	.string	"i"
	.byte	0x1
	.value	0x165
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x2d
	.long	0x424
	.byte	0x1
	.value	0x174
	.long	.LFB9
	.long	.LFE9
	.long	.LLST9
	.long	0x904
	.byte	0x1
	.long	0x958
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x29
	.long	.LASF75
	.byte	0x1
	.value	0x174
	.long	0x5b
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x22
	.long	.LBB23
	.long	.LBE23
	.uleb128 0x2a
	.long	.LASF77
	.byte	0x1
	.value	0x178
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x2a
	.long	.LASF78
	.byte	0x1
	.value	0x183
	.long	0x49b
	.byte	0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x2a
	.long	.LASF79
	.byte	0x1
	.value	0x184
	.long	0x30
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x2d
	.long	0x470
	.byte	0x1
	.value	0x18b
	.long	.LFB10
	.long	.LFE10
	.long	.LLST10
	.long	0x975
	.byte	0x1
	.long	0x99a
	.uleb128 0x21
	.long	.LASF58
	.long	0x552
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x22
	.long	.LBB25
	.long	.LBE25
	.uleb128 0x2b
	.string	"i"
	.byte	0x1
	.value	0x18d
	.long	0x4a7
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.byte	0
	.uleb128 0x2e
	.long	.LASF80
	.byte	0x1
	.byte	0x4
	.long	0x2eb
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	rootAddressSpace
	.uleb128 0x2e
	.long	.LASF81
	.byte	0x1
	.byte	0x5
	.long	0x4ae
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	currentAddressSpace
	.uleb128 0x2f
	.long	.LASF82
	.byte	0x1
	.byte	0x8
	.long	0x1a4
	.byte	0x1
	.byte	0x1
	.uleb128 0x2f
	.long	.LASF83
	.byte	0x1
	.byte	0x9
	.long	0x2e0
	.byte	0x1
	.byte	0x1
	.uleb128 0x2f
	.long	.LASF84
	.byte	0x1
	.byte	0xc
	.long	0x45
	.byte	0x1
	.byte	0x1
	.uleb128 0x2f
	.long	.LASF85
	.byte	0x1
	.byte	0xd
	.long	0x45
	.byte	0x1
	.byte	0x1
	.uleb128 0x2e
	.long	.LASF86
	.byte	0x1
	.byte	0xf
	.long	0x4a7
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	availableMappingPages
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
	.uleb128 0x6
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x7
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
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
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
	.uleb128 0xd
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xe
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
	.uleb128 0xf
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
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x12
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
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x32
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
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
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x16
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
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
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
	.uleb128 0x19
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
	.uleb128 0x1a
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
	.uleb128 0x1b
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
	.uleb128 0x1c
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
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
	.uleb128 0x1e
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
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
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
	.uleb128 0x20
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
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x23
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
	.uleb128 0x24
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x25
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
	.uleb128 0x26
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x27
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
	.uleb128 0x28
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
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x29
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
	.uleb128 0x2a
	.uleb128 0x34
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
	.uleb128 0x2b
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x2c
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2d
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
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2e
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
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x2f
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
	.long	.LBB21-.Ltext0
	.long	.LBE21-.Ltext0
	.long	.LBB22-.Ltext0
	.long	.LBE22-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF30:
	.string	"WriteThough"
.LASF64:
	.string	"newLocation"
.LASF73:
	.string	"newPageInfo"
.LASF93:
	.string	"_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj"
.LASF72:
	.string	"pagOffset"
.LASF49:
	.string	"Unmap"
.LASF44:
	.string	"_ZN18SPageDirectoryInfo4InitEv"
.LASF3:
	.string	"size_t"
.LASF33:
	.string	"RawValue"
.LASF35:
	.string	"sizetype"
.LASF40:
	.string	"directoryPhysical"
.LASF41:
	.string	"InitKernel"
.LASF68:
	.string	"newPage"
.LASF38:
	.string	"tables"
.LASF22:
	.string	"Available"
.LASF61:
	.string	"physical"
.LASF51:
	.string	"Address"
.LASF14:
	.string	"fmPageData"
.LASF81:
	.string	"currentAddressSpace"
.LASF10:
	.string	"8MAP_TYPE"
.LASF66:
	.string	"foundLength"
.LASF62:
	.string	"count"
.LASF63:
	.string	"searchReverse"
.LASF77:
	.string	"directoryIndex"
.LASF82:
	.string	"BootPageDirectory"
.LASF9:
	.string	"MAP_TYPE"
.LASF56:
	.string	"bool"
.LASF19:
	.string	"WriteThrough"
.LASF94:
	.string	"_ZN18SPageDirectoryInfo3MapEiPvS0_"
.LASF25:
	.string	"UserData"
.LASF13:
	.string	"fmUser"
.LASF53:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPvb"
.LASF80:
	.string	"rootAddressSpace"
.LASF42:
	.string	"Init"
.LASF31:
	.string	"Dirty"
.LASF88:
	.string	"mem_logical.cpp"
.LASF7:
	.string	"pmApplication"
.LASF69:
	.string	"newTable"
.LASF92:
	.string	"revision"
.LASF47:
	.string	"Select"
.LASF26:
	.string	"Frame"
.LASF8:
	.string	"pmKernel"
.LASF87:
	.string	"GNU C++ 4.7.1 20120721 (prerelease)"
.LASF43:
	.string	"_ZN18SPageDirectoryInfo10InitKernelEv"
.LASF39:
	.string	"tablesPhysical"
.LASF74:
	.string	"newDirectoryEntry"
.LASF24:
	.string	"Global"
.LASF20:
	.string	"CacheDisable"
.LASF29:
	.string	"5SPage"
.LASF32:
	.string	"PatIndex"
.LASF89:
	.string	"/home/colin/Development/OSdevelopment/Kernel/Kernel/temp"
.LASF1:
	.string	"unsigned char"
.LASF67:
	.string	"pageDirEntry"
.LASF28:
	.string	"14SPageDirectory"
.LASF16:
	.string	"Present"
.LASF65:
	.string	"currentLocation"
.LASF0:
	.string	"unsigned int"
.LASF34:
	.string	"SPage"
.LASF90:
	.string	"10SPageTable"
.LASF46:
	.string	"_ZN18SPageDirectoryInfo7ReleaseEv"
.LASF2:
	.string	"short unsigned int"
.LASF55:
	.string	"_ZN18SPageDirectoryInfo10CopyKernelEv"
.LASF71:
	.string	"pagEntry"
.LASF59:
	.string	"permissions"
.LASF75:
	.string	"logical"
.LASF57:
	.string	"bootstrapPhysical"
.LASF86:
	.string	"availableMappingPages"
.LASF95:
	.string	"InvalidateTLB"
.LASF58:
	.string	"this"
.LASF50:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPv"
.LASF45:
	.string	"Release"
.LASF4:
	.string	"UInt32"
.LASF85:
	.string	"phys"
.LASF6:
	.string	"long unsigned int"
.LASF70:
	.string	"dirEntry"
.LASF52:
	.string	"_ZN18SPageDirectoryInfo7AddressEPv"
.LASF18:
	.string	"User"
.LASF21:
	.string	"Accessed"
.LASF27:
	.string	"SPageDirectory"
.LASF60:
	.string	"location"
.LASF23:
	.string	"PageSize"
.LASF83:
	.string	"PageMapBootstrap"
.LASF12:
	.string	"fmWritable"
.LASF79:
	.string	"tableIndex"
.LASF48:
	.string	"_ZN18SPageDirectoryInfo6SelectEv"
.LASF54:
	.string	"CopyKernel"
.LASF78:
	.string	"table"
.LASF17:
	.string	"Writable"
.LASF91:
	.string	"SPageDirectoryInfo"
.LASF5:
	.string	"PhysicalPointer"
.LASF37:
	.string	"pages"
.LASF36:
	.string	"SPageTable"
.LASF84:
	.string	"virt"
.LASF15:
	.string	"MAP_FLAGS"
.LASF76:
	.string	"mapPage"
.LASF11:
	.string	"9MAP_FLAGS"
	.ident	"GCC: (GNU) 4.7.1 20120721 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
