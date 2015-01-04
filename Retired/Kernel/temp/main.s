	.file	"main.cpp"
	.text
.Ltext0:
	.type	_ZL10FixAddressPv, @function
_ZL10FixAddressPv:
.LFB7:
	.file 1 "main.cpp"
	.loc 1 12 0
	.cfi_startproc
	pushl	%ebp
.LCFI0:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI1:
	.cfi_def_cfa_register 5
	.loc 1 13 0
	movl	$virt, %edx
	movl	$phys, %eax
	subl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	.loc 1 14 0
	popl	%ebp
.LCFI2:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	_ZL10FixAddressPv, .-_ZL10FixAddressPv
	.section	.rodata
	.align 4
.LC0:
	.string	"Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2013 Colin Munro\n\n"
	.align 4
.LC1:
	.string	"Multiboot: magic number incorrect\n"
	.align 4
.LC2:
	.string	"Multiboot: %iKb low mem, %iKb high mem:\n"
.LC3:
	.string	"\t0x%.8x: %i bytes\n"
.LC4:
	.string	"Reserving...\n"
.LC5:
	.string	"Stack at 0x%.8x\n"
.LC6:
	.string	"\nStarting!\n"
	.text
	.globl	k_main
	.type	k_main, @function
k_main:
.LFB8:
	.loc 1 17 0
	.cfi_startproc
	pushl	%ebp
.LCFI3:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI4:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB2:
	.loc 1 20 0
	movl	$65, (%esp)
	call	_Z4testc
	.loc 1 21 0
	movl	$.LC0, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 22 0
	cmpl	$732803074, 12(%ebp)
	je	.L4
	.loc 1 24 0
	movl	$.LC1, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 25 0
	movl	$-1, %eax
	jmp	.L5
.L4:
	.loc 1 29 0
	call	_ZN15CPhysicalMemory4InitEv
	.loc 1 30 0
	movl	$rootAddressSpace, (%esp)
	call	_ZN18SPageDirectoryInfo10InitKernelEv
	.loc 1 33 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZL10FixAddressPv
	movl	%eax, 8(%ebp)
	.loc 1 35 0
	movl	8(%ebp), %eax
	movl	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 37 0
	movl	8(%ebp), %eax
	movl	48(%eax), %eax
	movl	%eax, (%esp)
	call	_ZL10FixAddressPv
	movl	%eax, -12(%ebp)
	.loc 1 36 0
	jmp	.L6
.L8:
	.loc 1 41 0
	movl	-12(%ebp), %eax
	movl	20(%eax), %eax
	cmpl	$1, %eax
	jne	.L7
	.loc 1 44 0
	movl	-12(%ebp), %eax
	movl	12(%eax), %edx
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC3, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 45 0
	movl	-12(%ebp), %eax
	movl	12(%eax), %edx
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_ZN15CPhysicalMemory8AddChunkEPvm
.L7:
	.loc 1 39 0
	movl	-12(%ebp), %eax
	movl	(%eax), %eax
	addl	$4, %eax
	.loc 1 36 0
	addl	%eax, -12(%ebp)
.L6:
	.loc 1 38 0 discriminator 1
	movl	8(%ebp), %eax
	movl	48(%eax), %eax
	movl	%eax, (%esp)
	call	_ZL10FixAddressPv
	.loc 1 36 0 discriminator 1
	movl	8(%ebp), %edx
	movl	44(%edx), %edx
	addl	%edx, %eax
	cmpl	-12(%ebp), %eax
	seta	%al
	testb	%al, %al
	jne	.L8
	.loc 1 48 0
	movl	$.LC4, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 49 0
	movl	$kern_end, %edx
	movl	$kern_start, %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, 4(%esp)
	movl	$kern_start, (%esp)
	call	_ZN15CPhysicalMemory11AddReservedEPvm
	.loc 1 50 0
	movl	$stack, 4(%esp)
	movl	$.LC5, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 52 0
	call	_Z9InitDebugv
	.loc 1 55 0
	movl	$.LC6, (%esp)
	call	_Z7kprintfPKcz
	.loc 1 56 0
	movl	$90, (%esp)
	call	_Z4testc
	.loc 1 57 0
	movl	$0, %eax
.L5:
.LBE2:
	.loc 1 58 0
	leave
.LCFI5:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	k_main, .-k_main
.Letext0:
	.file 2 "multiboot.h"
	.file 3 "runtime.h"
	.file 4 "mem_physical.h"
	.file 5 "mem_logical.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x739
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF90
	.byte	0x4
	.long	.LASF91
	.long	.LASF92
	.long	.Ltext0
	.long	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF31
	.uleb128 0x3
	.long	.LASF4
	.byte	0x10
	.byte	0x2
	.byte	0x3e
	.long	0x71
	.uleb128 0x4
	.long	.LASF0
	.byte	0x2
	.byte	0x40
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x4
	.long	.LASF1
	.byte	0x2
	.byte	0x41
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x4
	.long	.LASF2
	.byte	0x2
	.byte	0x42
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x4
	.long	.LASF3
	.byte	0x2
	.byte	0x43
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0
	.uleb128 0x5
	.long	.LASF8
	.byte	0x2
	.byte	0x44
	.long	0x2c
	.uleb128 0x3
	.long	.LASF5
	.byte	0x10
	.byte	0x2
	.byte	0x47
	.long	0xc1
	.uleb128 0x6
	.string	"num"
	.byte	0x2
	.byte	0x49
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x4
	.long	.LASF6
	.byte	0x2
	.byte	0x4a
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x4
	.long	.LASF2
	.byte	0x2
	.byte	0x4b
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x4
	.long	.LASF7
	.byte	0x2
	.byte	0x4c
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0
	.uleb128 0x5
	.long	.LASF9
	.byte	0x2
	.byte	0x4d
	.long	0x7c
	.uleb128 0x3
	.long	.LASF10
	.byte	0x34
	.byte	0x2
	.byte	0x50
	.long	0x182
	.uleb128 0x7
	.byte	0x10
	.byte	0x2
	.byte	0x5a
	.long	0xf7
	.uleb128 0x8
	.long	.LASF11
	.byte	0x2
	.byte	0x5b
	.long	0x71
	.uleb128 0x8
	.long	.LASF12
	.byte	0x2
	.byte	0x5c
	.long	0xc1
	.byte	0
	.uleb128 0x4
	.long	.LASF13
	.byte	0x2
	.byte	0x52
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x4
	.long	.LASF14
	.byte	0x2
	.byte	0x53
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x4
	.long	.LASF15
	.byte	0x2
	.byte	0x54
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x4
	.long	.LASF16
	.byte	0x2
	.byte	0x55
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x4
	.long	.LASF17
	.byte	0x2
	.byte	0x56
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x4
	.long	.LASF18
	.byte	0x2
	.byte	0x57
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x4
	.long	.LASF19
	.byte	0x2
	.byte	0x58
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x6
	.string	"u"
	.byte	0x2
	.byte	0x5d
	.long	0xd8
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x4
	.long	.LASF20
	.byte	0x2
	.byte	0x5e
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x4
	.long	.LASF21
	.byte	0x2
	.byte	0x5f
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.byte	0
	.uleb128 0x5
	.long	.LASF22
	.byte	0x2
	.byte	0x60
	.long	0xcc
	.uleb128 0x3
	.long	.LASF23
	.byte	0x18
	.byte	0x2
	.byte	0x6d
	.long	0x1ee
	.uleb128 0x4
	.long	.LASF6
	.byte	0x2
	.byte	0x6f
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x4
	.long	.LASF24
	.byte	0x2
	.byte	0x70
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x4
	.long	.LASF25
	.byte	0x2
	.byte	0x71
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x4
	.long	.LASF26
	.byte	0x2
	.byte	0x72
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x4
	.long	.LASF27
	.byte	0x2
	.byte	0x73
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x4
	.long	.LASF28
	.byte	0x2
	.byte	0x74
	.long	0x25
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.byte	0
	.uleb128 0x5
	.long	.LASF29
	.byte	0x2
	.byte	0x75
	.long	0x18d
	.uleb128 0x5
	.long	.LASF30
	.byte	0x3
	.byte	0x8
	.long	0x204
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF32
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF33
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF34
	.uleb128 0x5
	.long	.LASF35
	.byte	0x3
	.byte	0xc
	.long	0x204
	.uleb128 0x5
	.long	.LASF36
	.byte	0x4
	.byte	0x6
	.long	0x22f
	.uleb128 0x9
	.byte	0x4
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF37
	.uleb128 0xa
	.byte	0x4
	.byte	0x5
	.byte	0x7
	.long	.LASF93
	.long	0x251
	.uleb128 0xb
	.long	.LASF38
	.sleb128 0
	.uleb128 0xb
	.long	.LASF39
	.sleb128 1
	.byte	0
	.uleb128 0x5
	.long	.LASF40
	.byte	0x5
	.byte	0xa
	.long	0x238
	.uleb128 0xc
	.byte	0x4
	.byte	0x5
	.byte	0x12
	.long	.LASF54
	.long	0x34e
	.uleb128 0x7
	.byte	0x4
	.byte	0x5
	.byte	0x13
	.long	0x345
	.uleb128 0xd
	.byte	0x4
	.byte	0x5
	.byte	0x14
	.long	0x334
	.uleb128 0xe
	.long	.LASF41
	.byte	0x5
	.byte	0x15
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF42
	.byte	0x5
	.byte	0x16
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF43
	.byte	0x5
	.byte	0x17
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF44
	.byte	0x5
	.byte	0x18
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF45
	.byte	0x5
	.byte	0x19
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF46
	.byte	0x5
	.byte	0x1a
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF47
	.byte	0x5
	.byte	0x1b
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF48
	.byte	0x5
	.byte	0x1c
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF49
	.byte	0x5
	.byte	0x1d
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF50
	.byte	0x5
	.byte	0x1e
	.long	0x219
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF51
	.byte	0x5
	.byte	0x1f
	.long	0x219
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0xf
	.long	0x270
	.uleb128 0x8
	.long	.LASF52
	.byte	0x5
	.byte	0x21
	.long	0x219
	.byte	0
	.uleb128 0x10
	.long	0x268
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x5
	.long	.LASF53
	.byte	0x5
	.byte	0x23
	.long	0x25c
	.uleb128 0xc
	.byte	0x4
	.byte	0x5
	.byte	0x25
	.long	.LASF55
	.long	0x44b
	.uleb128 0x7
	.byte	0x4
	.byte	0x5
	.byte	0x26
	.long	0x442
	.uleb128 0xd
	.byte	0x4
	.byte	0x5
	.byte	0x27
	.long	0x431
	.uleb128 0xe
	.long	.LASF41
	.byte	0x5
	.byte	0x28
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF42
	.byte	0x5
	.byte	0x29
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF43
	.byte	0x5
	.byte	0x2a
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF56
	.byte	0x5
	.byte	0x2b
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF45
	.byte	0x5
	.byte	0x2c
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF46
	.byte	0x5
	.byte	0x2d
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF57
	.byte	0x5
	.byte	0x2e
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF58
	.byte	0x5
	.byte	0x2f
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF49
	.byte	0x5
	.byte	0x30
	.long	0x219
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF50
	.byte	0x5
	.byte	0x31
	.long	0x219
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xe
	.long	.LASF51
	.byte	0x5
	.byte	0x32
	.long	0x219
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0xf
	.long	0x36d
	.uleb128 0x8
	.long	.LASF52
	.byte	0x5
	.byte	0x34
	.long	0x219
	.byte	0
	.uleb128 0x10
	.long	0x365
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x5
	.long	.LASF59
	.byte	0x5
	.byte	0x36
	.long	0x359
	.uleb128 0x11
	.value	0x1000
	.byte	0x5
	.byte	0x38
	.long	.LASF94
	.long	0x472
	.uleb128 0x4
	.long	.LASF60
	.byte	0x5
	.byte	0x39
	.long	0x472
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x12
	.long	0x44b
	.long	0x483
	.uleb128 0x13
	.long	0x483
	.value	0x3ff
	.byte	0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF61
	.uleb128 0x5
	.long	.LASF62
	.byte	0x5
	.byte	0x3a
	.long	0x456
	.uleb128 0x14
	.long	.LASF95
	.value	0x100c
	.byte	0x5
	.byte	0x3c
	.long	0x634
	.uleb128 0x4
	.long	.LASF63
	.byte	0x5
	.byte	0x40
	.long	0x634
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x4
	.long	.LASF64
	.byte	0x5
	.byte	0x41
	.long	0x64b
	.byte	0x3
	.byte	0x23
	.uleb128 0x1000
	.uleb128 0x4
	.long	.LASF65
	.byte	0x5
	.byte	0x42
	.long	0x224
	.byte	0x3
	.byte	0x23
	.uleb128 0x1004
	.uleb128 0x15
	.long	.LASF96
	.byte	0x5
	.byte	0x53
	.long	0x651
	.byte	0x3
	.byte	0x23
	.uleb128 0x1008
	.byte	0x3
	.uleb128 0x16
	.byte	0x1
	.long	.LASF66
	.byte	0x5
	.byte	0x44
	.long	.LASF68
	.byte	0x1
	.long	0x4f3
	.long	0x4fa
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.byte	0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF67
	.byte	0x5
	.byte	0x46
	.long	.LASF69
	.byte	0x1
	.long	0x50f
	.long	0x516
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.byte	0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF70
	.byte	0x5
	.byte	0x47
	.long	.LASF71
	.byte	0x1
	.long	0x52b
	.long	0x532
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.byte	0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF72
	.byte	0x5
	.byte	0x48
	.long	.LASF73
	.byte	0x1
	.long	0x547
	.long	0x54e
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.byte	0
	.uleb128 0x18
	.byte	0x1
	.string	"Map"
	.byte	0x5
	.byte	0x4a
	.long	.LASF97
	.long	0x22f
	.byte	0x1
	.long	0x567
	.long	0x582
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.uleb128 0x19
	.long	0x651
	.uleb128 0x19
	.long	0x251
	.uleb128 0x19
	.long	0x224
	.uleb128 0x19
	.long	0x1f9
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.string	"Map"
	.byte	0x5
	.byte	0x4b
	.long	.LASF98
	.byte	0x1
	.long	0x597
	.long	0x5ad
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.uleb128 0x19
	.long	0x651
	.uleb128 0x19
	.long	0x22f
	.uleb128 0x19
	.long	0x224
	.byte	0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF74
	.byte	0x5
	.byte	0x4c
	.long	.LASF75
	.byte	0x1
	.long	0x5c2
	.long	0x5ce
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.uleb128 0x19
	.long	0x22f
	.byte	0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF76
	.byte	0x5
	.byte	0x4d
	.long	.LASF77
	.long	0x224
	.byte	0x1
	.long	0x5e7
	.long	0x5f3
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.uleb128 0x19
	.long	0x22f
	.byte	0
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF74
	.byte	0x5
	.byte	0x50
	.long	.LASF78
	.byte	0x3
	.byte	0x1
	.long	0x609
	.long	0x61a
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.uleb128 0x19
	.long	0x22f
	.uleb128 0x19
	.long	0x65e
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF79
	.byte	0x5
	.byte	0x51
	.long	.LASF80
	.byte	0x3
	.byte	0x1
	.long	0x62c
	.uleb128 0x17
	.long	0x658
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x12
	.long	0x645
	.long	0x645
	.uleb128 0x13
	.long	0x483
	.value	0x3ff
	.byte	0
	.uleb128 0x1e
	.byte	0x4
	.long	0x48a
	.uleb128 0x1e
	.byte	0x4
	.long	0x34e
	.uleb128 0x1f
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x1e
	.byte	0x4
	.long	0x495
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.long	.LASF81
	.uleb128 0x20
	.long	.LASF99
	.byte	0x1
	.byte	0xb
	.long	0x22f
	.long	.LFB7
	.long	.LFE7
	.long	.LLST0
	.byte	0x1
	.long	0x690
	.uleb128 0x21
	.long	.LASF82
	.byte	0x1
	.byte	0xb
	.long	0x22f
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x22
	.byte	0x1
	.long	.LASF100
	.byte	0x1
	.byte	0x10
	.long	0x651
	.long	.LFB8
	.long	.LFE8
	.long	.LLST1
	.byte	0x1
	.long	0x6e2
	.uleb128 0x23
	.string	"mbd"
	.byte	0x1
	.byte	0x10
	.long	0x6e2
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x21
	.long	.LASF83
	.byte	0x1
	.byte	0x10
	.long	0x204
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x24
	.long	.LBB2
	.long	.LBE2
	.uleb128 0x25
	.string	"map"
	.byte	0x1
	.byte	0x12
	.long	0x6e8
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x1e
	.byte	0x4
	.long	0x182
	.uleb128 0x1e
	.byte	0x4
	.long	0x1ee
	.uleb128 0x26
	.long	.LASF84
	.byte	0x5
	.byte	0x56
	.long	0x495
	.byte	0x1
	.byte	0x1
	.uleb128 0x26
	.long	.LASF85
	.byte	0x1
	.byte	0x7
	.long	0x219
	.byte	0x1
	.byte	0x1
	.uleb128 0x26
	.long	.LASF86
	.byte	0x1
	.byte	0x7
	.long	0x219
	.byte	0x1
	.byte	0x1
	.uleb128 0x26
	.long	.LASF87
	.byte	0x1
	.byte	0x9
	.long	0x219
	.byte	0x1
	.byte	0x1
	.uleb128 0x26
	.long	.LASF88
	.byte	0x1
	.byte	0x9
	.long	0x219
	.byte	0x1
	.byte	0x1
	.uleb128 0x26
	.long	.LASF89
	.byte	0x1
	.byte	0xf
	.long	0x219
	.byte	0x1
	.byte	0x1
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
	.uleb128 0x3
	.uleb128 0x13
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
	.uleb128 0x4
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
	.uleb128 0x5
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
	.uleb128 0x6
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x7
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
	.uleb128 0x8
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
	.uleb128 0x9
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
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
	.uleb128 0xb
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0xc
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
	.uleb128 0xd
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
	.uleb128 0xe
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
	.uleb128 0xf
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x11
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
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x14
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
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x32
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x16
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
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
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
	.uleb128 0x19
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
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
	.uleb128 0x1c
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
	.uleb128 0x32
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
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
	.uleb128 0x20
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x21
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
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x23
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
	.uleb128 0x24
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
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
	.long	.LFB7-.Ltext0
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
	.long	.LFE7-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST1:
	.long	.LFB8-.Ltext0
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
	.long	.LFE8-.Ltext0
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
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF9:
	.string	"elf_section_header_table_t"
.LASF4:
	.string	"aout_symbol_table"
.LASF97:
	.string	"_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj"
.LASF88:
	.string	"phys"
.LASF74:
	.string	"Unmap"
.LASF69:
	.string	"_ZN18SPageDirectoryInfo4InitEv"
.LASF30:
	.string	"size_t"
.LASF52:
	.string	"RawValue"
.LASF61:
	.string	"sizetype"
.LASF84:
	.string	"rootAddressSpace"
.LASF65:
	.string	"directoryPhysical"
.LASF66:
	.string	"InitKernel"
.LASF70:
	.string	"Release"
.LASF17:
	.string	"cmdline"
.LASF47:
	.string	"Available"
.LASF11:
	.string	"aout_sym"
.LASF76:
	.string	"Address"
.LASF19:
	.string	"mods_addr"
.LASF64:
	.string	"tablesPhysical"
.LASF1:
	.string	"strsize"
.LASF82:
	.string	"address"
.LASF93:
	.string	"8MAP_TYPE"
.LASF48:
	.string	"PageSize"
.LASF99:
	.string	"FixAddress"
.LASF7:
	.string	"shndx"
.LASF81:
	.string	"bool"
.LASF44:
	.string	"WriteThrough"
.LASF98:
	.string	"_ZN18SPageDirectoryInfo3MapEiPvS0_"
.LASF50:
	.string	"UserData"
.LASF23:
	.string	"memory_map"
.LASF78:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPvb"
.LASF56:
	.string	"WriteThough"
.LASF67:
	.string	"Init"
.LASF57:
	.string	"Dirty"
.LASF89:
	.string	"stack"
.LASF38:
	.string	"pmApplication"
.LASF27:
	.string	"length_high"
.LASF2:
	.string	"addr"
.LASF72:
	.string	"Select"
.LASF100:
	.string	"k_main"
.LASF51:
	.string	"Frame"
.LASF39:
	.string	"pmKernel"
.LASF90:
	.string	"GNU C++ 4.7.1 20120721 (prerelease)"
.LASF8:
	.string	"aout_symbol_table_t"
.LASF79:
	.string	"CopyKernel"
.LASF49:
	.string	"Global"
.LASF54:
	.string	"14SPageDirectory"
.LASF45:
	.string	"CacheDisable"
.LASF21:
	.string	"mmap_addr"
.LASF58:
	.string	"PatIndex"
.LASF92:
	.string	"/home/colin/Development/OSdevelopment/Kernel/Kernel/temp"
.LASF33:
	.string	"unsigned char"
.LASF40:
	.string	"MAP_TYPE"
.LASF41:
	.string	"Present"
.LASF12:
	.string	"elf_sec"
.LASF14:
	.string	"mem_lower"
.LASF13:
	.string	"flags"
.LASF3:
	.string	"reserved"
.LASF28:
	.string	"type"
.LASF32:
	.string	"unsigned int"
.LASF59:
	.string	"SPage"
.LASF20:
	.string	"mmap_length"
.LASF94:
	.string	"10SPageTable"
.LASF24:
	.string	"base_addr_low"
.LASF71:
	.string	"_ZN18SPageDirectoryInfo7ReleaseEv"
.LASF85:
	.string	"kern_start"
.LASF34:
	.string	"short unsigned int"
.LASF80:
	.string	"_ZN18SPageDirectoryInfo10CopyKernelEv"
.LASF37:
	.string	"char"
.LASF16:
	.string	"boot_device"
.LASF10:
	.string	"multiboot_info"
.LASF75:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPv"
.LASF86:
	.string	"kern_end"
.LASF15:
	.string	"mem_upper"
.LASF35:
	.string	"UInt32"
.LASF68:
	.string	"_ZN18SPageDirectoryInfo10InitKernelEv"
.LASF96:
	.string	"revision"
.LASF31:
	.string	"long unsigned int"
.LASF77:
	.string	"_ZN18SPageDirectoryInfo7AddressEPv"
.LASF0:
	.string	"tabsize"
.LASF6:
	.string	"size"
.LASF43:
	.string	"User"
.LASF46:
	.string	"Accessed"
.LASF53:
	.string	"SPageDirectory"
.LASF26:
	.string	"length_low"
.LASF5:
	.string	"elf_section_header_table"
.LASF22:
	.string	"multiboot_info_t"
.LASF73:
	.string	"_ZN18SPageDirectoryInfo6SelectEv"
.LASF18:
	.string	"mods_count"
.LASF25:
	.string	"base_addr_high"
.LASF83:
	.string	"magic"
.LASF42:
	.string	"Writable"
.LASF95:
	.string	"SPageDirectoryInfo"
.LASF29:
	.string	"memory_map_t"
.LASF36:
	.string	"PhysicalPointer"
.LASF63:
	.string	"tables"
.LASF91:
	.string	"main.cpp"
.LASF60:
	.string	"pages"
.LASF62:
	.string	"SPageTable"
.LASF87:
	.string	"virt"
.LASF55:
	.string	"5SPage"
	.ident	"GCC: (GNU) 4.7.1 20120721 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
