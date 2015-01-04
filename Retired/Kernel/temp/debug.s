	.file	"debug.cpp"
	.text
.Ltext0:
	.type	_ZL4outbth, @function
_ZL4outbth:
.LFB0:
	.file 1 "tools.h"
	.loc 1 33 0
	.cfi_startproc
	pushl	%ebp
.LCFI0:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI1:
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movw	%dx, -4(%ebp)
	movb	%al, -8(%ebp)
	.loc 1 34 0
	movzbl	-8(%ebp), %eax
	movzwl	-4(%ebp), %edx
#APP
# 34 "tools.h" 1
	outb %al,%dx
# 0 "" 2
	.loc 1 35 0
#NO_APP
	leave
.LCFI2:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	_ZL4outbth, .-_ZL4outbth
	.type	_ZL3inbt, @function
_ZL3inbt:
.LFB3:
	.loc 1 48 0
	.cfi_startproc
	pushl	%ebp
.LCFI3:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI4:
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset 3, -12
	movl	8(%ebp), %eax
	movw	%ax, -24(%ebp)
.LBB2:
	.loc 1 50 0
	movzwl	-24(%ebp), %edx
	movw	%dx, -22(%ebp)
	movzwl	-22(%ebp), %edx
#APP
# 50 "tools.h" 1
	inb %dx,%al
# 0 "" 2
#NO_APP
	movl	%eax, %ebx
	movb	%bl, -5(%ebp)
	.loc 1 51 0
	movzbl	-5(%ebp), %eax
.LBE2:
	.loc 1 52 0
	addl	$20, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI5:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	_ZL3inbt, .-_ZL3inbt
	.local	_ZL13s_framebuffer
	.comm	_ZL13s_framebuffer,4,4
	.type	_ZL15Get_Framebufferv, @function
_ZL15Get_Framebufferv:
.LFB7:
	.file 2 "debug.cpp"
	.loc 2 13 0
	.cfi_startproc
	pushl	%ebp
.LCFI6:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI7:
	.cfi_def_cfa_register 5
	pushl	%ebx
	.cfi_offset 3, -12
	.loc 2 15 0
	movl	_ZL13s_framebuffer, %eax
	testl	%eax, %eax
	jne	.L5
	.loc 2 16 0
	movl	$virt, %eax
	movl	%eax, %edx
	sarl	$31, %edx
	movl	$phys, %ecx
	movl	%ecx, %ebx
	sarl	$31, %ebx
	subl	%ecx, %eax
	sbbl	%ebx, %edx
	addl	$753664, %eax
	movl	%eax, _ZL13s_framebuffer
.L5:
	.loc 2 17 0
	movl	_ZL13s_framebuffer, %eax
	.loc 2 18 0
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI8:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	_ZL15Get_Framebufferv, .-_ZL15Get_Framebufferv
	.section	.rodata
.LC0:
	.string	"Framebuffer address: 0x%.8x\n"
	.text
	.globl	_Z9InitDebugv
	.type	_Z9InitDebugv, @function
_Z9InitDebugv:
.LFB8:
	.loc 2 21 0
	.cfi_startproc
	pushl	%ebp
.LCFI9:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI10:
	.cfi_def_cfa_register 5
	subl	$40, %esp
	.loc 2 22 0
	call	_ZL15Get_Framebufferv
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	_Z7kprintfPKcz
	.loc 2 23 0
	movl	$1, 16(%esp)
	movl	$753664, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1, 4(%esp)
	movl	$rootAddressSpace, (%esp)
	call	_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj
	movl	%eax, _ZL13s_framebuffer
	.loc 2 24 0
	call	_ZL15Get_Framebufferv
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	_Z7kprintfPKcz
	.loc 2 25 0
	leave
.LCFI11:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	_Z9InitDebugv, .-_Z9InitDebugv
	.globl	_Z4testc
	.type	_Z4testc, @function
_Z4testc:
.LFB9:
	.loc 2 28 0
	.cfi_startproc
	pushl	%ebp
.LCFI12:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI13:
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %eax
	movb	%al, -20(%ebp)
.LBB3:
	.loc 2 29 0
	call	_ZL15Get_Framebufferv
	addl	$3998, %eax
	movl	%eax, -4(%ebp)
	.loc 2 30 0
	movl	-4(%ebp), %eax
	movzbl	-20(%ebp), %edx
	movb	%dl, (%eax)
	.loc 2 31 0
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movb	$7, (%eax)
.LBE3:
	.loc 2 32 0
	leave
.LCFI14:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	_Z4testc, .-_Z4testc
	.section	.text._ZN14CathodeRayTubeC2Ev,"axG",@progbits,_ZN14CathodeRayTubeC5Ev,comdat
	.align 2
	.weak	_ZN14CathodeRayTubeC2Ev
	.type	_ZN14CathodeRayTubeC2Ev, @function
_ZN14CathodeRayTubeC2Ev:
.LFB11:
	.loc 2 40 0
	.cfi_startproc
	pushl	%ebp
.LCFI15:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI16:
	.cfi_def_cfa_register 5
.LBB4:
	.loc 2 42 0
	movl	8(%ebp), %eax
	movb	$0, (%eax)
.LBE4:
	.loc 2 43 0
	popl	%ebp
.LCFI17:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE11:
	.size	_ZN14CathodeRayTubeC2Ev, .-_ZN14CathodeRayTubeC2Ev
	.weak	_ZN14CathodeRayTubeC1Ev
	.set	_ZN14CathodeRayTubeC1Ev,_ZN14CathodeRayTubeC2Ev
	.section	.text._ZN14CathodeRayTubeD2Ev,"axG",@progbits,_ZN14CathodeRayTubeD5Ev,comdat
	.align 2
	.weak	_ZN14CathodeRayTubeD2Ev
	.type	_ZN14CathodeRayTubeD2Ev, @function
_ZN14CathodeRayTubeD2Ev:
.LFB14:
	.loc 2 44 0
	.cfi_startproc
	pushl	%ebp
.LCFI18:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI19:
	.cfi_def_cfa_register 5
	.loc 2 46 0
	popl	%ebp
.LCFI20:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE14:
	.size	_ZN14CathodeRayTubeD2Ev, .-_ZN14CathodeRayTubeD2Ev
	.weak	_ZN14CathodeRayTubeD1Ev
	.set	_ZN14CathodeRayTubeD1Ev,_ZN14CathodeRayTubeD2Ev
	.section	.text._ZN14CathodeRayTube5WidthEv,"axG",@progbits,_ZN14CathodeRayTube5WidthEv,comdat
	.align 2
	.weak	_ZN14CathodeRayTube5WidthEv
	.type	_ZN14CathodeRayTube5WidthEv, @function
_ZN14CathodeRayTube5WidthEv:
.LFB16:
	.loc 2 49 0
	.cfi_startproc
	pushl	%ebp
.LCFI21:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI22:
	.cfi_def_cfa_register 5
	.loc 2 51 0
	movl	$80, %eax
	.loc 2 52 0
	popl	%ebp
.LCFI23:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE16:
	.size	_ZN14CathodeRayTube5WidthEv, .-_ZN14CathodeRayTube5WidthEv
	.section	.text._ZN14CathodeRayTube6HeightEv,"axG",@progbits,_ZN14CathodeRayTube6HeightEv,comdat
	.align 2
	.weak	_ZN14CathodeRayTube6HeightEv
	.type	_ZN14CathodeRayTube6HeightEv, @function
_ZN14CathodeRayTube6HeightEv:
.LFB17:
	.loc 2 53 0
	.cfi_startproc
	pushl	%ebp
.LCFI24:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI25:
	.cfi_def_cfa_register 5
	.loc 2 55 0
	movl	$25, %eax
	.loc 2 56 0
	popl	%ebp
.LCFI26:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE17:
	.size	_ZN14CathodeRayTube6HeightEv, .-_ZN14CathodeRayTube6HeightEv
	.section	.text._ZN14CathodeRayTube11FrameBufferEv,"axG",@progbits,_ZN14CathodeRayTube11FrameBufferEv,comdat
	.align 2
	.weak	_ZN14CathodeRayTube11FrameBufferEv
	.type	_ZN14CathodeRayTube11FrameBufferEv, @function
_ZN14CathodeRayTube11FrameBufferEv:
.LFB18:
	.loc 2 59 0
	.cfi_startproc
	pushl	%ebp
.LCFI27:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI28:
	.cfi_def_cfa_register 5
	.loc 2 61 0
	call	_ZL15Get_Framebufferv
	.loc 2 62 0
	popl	%ebp
.LCFI29:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE18:
	.size	_ZN14CathodeRayTube11FrameBufferEv, .-_ZN14CathodeRayTube11FrameBufferEv
	.section	.text._ZN14CathodeRayTube15GetCursorOffsetEv,"axG",@progbits,_ZN14CathodeRayTube15GetCursorOffsetEv,comdat
	.align 2
	.weak	_ZN14CathodeRayTube15GetCursorOffsetEv
	.type	_ZN14CathodeRayTube15GetCursorOffsetEv, @function
_ZN14CathodeRayTube15GetCursorOffsetEv:
.LFB19:
	.loc 2 65 0
	.cfi_startproc
	pushl	%ebp
.LCFI30:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI31:
	.cfi_def_cfa_register 5
	subl	$24, %esp
.LBB5:
	.loc 2 69 0
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	je	.L19
	.loc 2 70 0
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	jmp	.L20
.L19:
	.loc 2 71 0
	movl	$14, 4(%esp)
	movl	$980, (%esp)
	call	_ZL4outbth
	.loc 2 72 0
	movl	$981, (%esp)
	call	_ZL3inbt
	movb	%al, -1(%ebp)
	.loc 2 73 0
	movl	$15, 4(%esp)
	movl	$980, (%esp)
	call	_ZL4outbth
	.loc 2 74 0
	movl	$981, (%esp)
	call	_ZL3inbt
	movb	%al, -2(%ebp)
	.loc 2 75 0
	movzbl	-1(%ebp), %eax
	movl	%eax, %edx
	sall	$8, %edx
	movzbl	-2(%ebp), %eax
	orl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%edx, 4(%eax)
	.loc 2 76 0
	movl	8(%ebp), %eax
	movb	$1, (%eax)
	.loc 2 77 0
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
.L20:
.LBE5:
	.loc 2 78 0
	leave
.LCFI32:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE19:
	.size	_ZN14CathodeRayTube15GetCursorOffsetEv, .-_ZN14CathodeRayTube15GetCursorOffsetEv
	.section	.text._ZN14CathodeRayTube15SetCursorOffsetEi,"axG",@progbits,_ZN14CathodeRayTube15SetCursorOffsetEi,comdat
	.align 2
	.weak	_ZN14CathodeRayTube15SetCursorOffsetEi
	.type	_ZN14CathodeRayTube15SetCursorOffsetEi, @function
_ZN14CathodeRayTube15SetCursorOffsetEi:
.LFB20:
	.loc 2 81 0
	.cfi_startproc
	pushl	%ebp
.LCFI33:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI34:
	.cfi_def_cfa_register 5
	subl	$8, %esp
	.loc 2 83 0
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movl	%edx, 4(%eax)
	.loc 2 84 0
	movl	8(%ebp), %eax
	movb	$1, (%eax)
	.loc 2 85 0
	movl	$14, 4(%esp)
	movl	$980, (%esp)
	call	_ZL4outbth
	.loc 2 86 0
	movl	12(%ebp), %eax
	andl	$65280, %eax
	sarl	$8, %eax
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$981, (%esp)
	call	_ZL4outbth
	.loc 2 87 0
	movl	$15, 4(%esp)
	movl	$980, (%esp)
	call	_ZL4outbth
	.loc 2 88 0
	movl	12(%ebp), %eax
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$981, (%esp)
	call	_ZL4outbth
	.loc 2 89 0
	leave
.LCFI35:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE20:
	.size	_ZN14CathodeRayTube15SetCursorOffsetEi, .-_ZN14CathodeRayTube15SetCursorOffsetEi
	.section	.text._ZN14CathodeRayTube5ClearEv,"axG",@progbits,_ZN14CathodeRayTube5ClearEv,comdat
	.align 2
	.weak	_ZN14CathodeRayTube5ClearEv
	.type	_ZN14CathodeRayTube5ClearEv, @function
_ZN14CathodeRayTube5ClearEv:
.LFB23:
	.loc 2 105 0
	.cfi_startproc
	pushl	%ebp
.LCFI36:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI37:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB6:
	.loc 2 110 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube11FrameBufferEv
	movl	%eax, -16(%ebp)
	.loc 2 111 0
	movl	$0, -12(%ebp)
	jmp	.L23
.L24:
	.loc 2 112 0 discriminator 2
	movl	-12(%ebp), %eax
	leal	(%eax,%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movw	$1824, (%eax)
	.loc 2 111 0 discriminator 2
	addl	$1, -12(%ebp)
.L23:
	.loc 2 111 0 is_stmt 0 discriminator 1
	cmpl	$1999, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L24
.LBE6:
	.loc 2 113 0 is_stmt 1
	leave
.LCFI38:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE23:
	.size	_ZN14CathodeRayTube5ClearEv, .-_ZN14CathodeRayTube5ClearEv
	.section	.text._ZN13ScreenPrinter8PrintOutEPKci,"axG",@progbits,_ZN13ScreenPrinter8PrintOutEPKci,comdat
	.align 2
	.weak	_ZN13ScreenPrinter8PrintOutEPKci
	.type	_ZN13ScreenPrinter8PrintOutEPKci, @function
_ZN13ScreenPrinter8PrintOutEPKci:
.LFB24:
	.loc 2 119 0
	.cfi_startproc
	pushl	%ebp
.LCFI39:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI40:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB7:
.LBB8:
	.loc 2 121 0
	movl	$0, -12(%ebp)
	jmp	.L26
.L27:
	.loc 2 123 0 discriminator 2
	movl	-12(%ebp), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter10HandleCharEc
	.loc 2 125 0 discriminator 2
	movl	-12(%ebp), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$233, (%esp)
	call	_ZL4outbth
	.loc 2 121 0 discriminator 2
	addl	$1, -12(%ebp)
.L26:
	.loc 2 121 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %eax
	cmpl	16(%ebp), %eax
	setl	%al
	testb	%al, %al
	jne	.L27
.LBE8:
.LBE7:
	.loc 2 127 0 is_stmt 1
	leave
.LCFI41:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE24:
	.size	_ZN13ScreenPrinter8PrintOutEPKci, .-_ZN13ScreenPrinter8PrintOutEPKci
	.section	.text._ZN13ScreenPrinter10HandleCharEc,"axG",@progbits,_ZN13ScreenPrinter10HandleCharEc,comdat
	.align 2
	.weak	_ZN13ScreenPrinter10HandleCharEc
	.type	_ZN13ScreenPrinter10HandleCharEc, @function
_ZN13ScreenPrinter10HandleCharEc:
.LFB25:
	.loc 2 129 0
	.cfi_startproc
	pushl	%ebp
.LCFI42:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI43:
	.cfi_def_cfa_register 5
	subl	$88, %esp
	movl	12(%ebp), %eax
	movb	%al, -60(%ebp)
.LBB9:
.LBB10:
	.loc 2 131 0
	movsbl	-60(%ebp), %eax
	cmpl	$10, %eax
	je	.L31
	cmpl	$10, %eax
	jg	.L34
	cmpl	$9, %eax
	je	.L30
	jmp	.L29
.L34:
	cmpl	$12, %eax
	je	.L32
	cmpl	$13, %eax
	je	.L33
	jmp	.L29
.L30:
.LBB11:
.LBB12:
	.loc 2 134 0
	movl	$0, -12(%ebp)
	jmp	.L35
.L36:
	.loc 2 135 0 discriminator 2
	movl	$7, 8(%esp)
	movl	$32, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter9PrintCharEci
	.loc 2 134 0 discriminator 2
	addl	$1, -12(%ebp)
.L35:
	.loc 2 134 0 is_stmt 0 discriminator 1
	cmpl	$3, -12(%ebp)
	setle	%al
	testb	%al, %al
	jne	.L36
.LBE12:
	.loc 2 136 0 is_stmt 1
	jmp	.L28
.L33:
.LBB13:
	.loc 2 139 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeC1Ev
	.loc 2 140 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube15GetCursorOffsetEv
	movl	%eax, -16(%ebp)
	.loc 2 141 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	movl	%eax, -64(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, %edx
	sarl	$31, %edx
	idivl	-64(%ebp)
	movl	%edx, %ecx
	movl	%ecx, %eax
	subl	%eax, -16(%ebp)
	.loc 2 142 0
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter9PrintCharEci
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeD1Ev
.LBE13:
	.loc 2 144 0
	jmp	.L28
.L32:
.LBB14:
	.loc 2 147 0
	leal	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeC1Ev
	.loc 2 148 0
	leal	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube15GetCursorOffsetEv
	movl	%eax, -20(%ebp)
	.loc 2 149 0
	leal	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	addl	%eax, -20(%ebp)
	.loc 2 150 0
	movl	-20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter9PrintCharEci
	leal	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeD1Ev
.LBE14:
	.loc 2 152 0
	jmp	.L28
.L31:
.LBB15:
	.loc 2 155 0
	leal	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeC1Ev
	.loc 2 156 0
	leal	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube15GetCursorOffsetEv
	movl	%eax, -24(%ebp)
	.loc 2 157 0
	leal	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	addl	%eax, -24(%ebp)
	.loc 2 158 0
	leal	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	movl	%eax, -64(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, %edx
	sarl	$31, %edx
	idivl	-64(%ebp)
	movl	%edx, %ecx
	movl	%ecx, %eax
	subl	%eax, -24(%ebp)
	.loc 2 159 0
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter9PrintCharEci
	leal	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeD1Ev
.LBE15:
	.loc 2 161 0
	jmp	.L28
.L29:
	.loc 2 163 0
	movsbl	-60(%ebp), %eax
	movl	$7, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinter9PrintCharEci
	.loc 2 164 0
	nop
.L28:
.LBE11:
.LBE10:
.LBE9:
	.loc 2 166 0
	leave
.LCFI44:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE25:
	.size	_ZN13ScreenPrinter10HandleCharEc, .-_ZN13ScreenPrinter10HandleCharEc
	.section	.text._ZN13ScreenPrinter9PrintCharEci,"axG",@progbits,_ZN13ScreenPrinter9PrintCharEci,comdat
	.align 2
	.weak	_ZN13ScreenPrinter9PrintCharEci
	.type	_ZN13ScreenPrinter9PrintCharEci, @function
_ZN13ScreenPrinter9PrintCharEci:
.LFB26:
	.loc 2 167 0
	.cfi_startproc
	pushl	%ebp
.LCFI45:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI46:
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$68, %esp
	.cfi_offset 3, -12
	movl	12(%ebp), %eax
	movb	%al, -44(%ebp)
.LBB16:
	.loc 2 169 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeC1Ev
	.loc 2 173 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube11FrameBufferEv
	movl	%eax, -20(%ebp)
	.loc 2 174 0
	cmpb	$0, -44(%ebp)
	je	.L39
	.loc 2 176 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube15GetCursorOffsetEv
	movl	%eax, -12(%ebp)
	.loc 2 177 0
	movl	-12(%ebp), %eax
	addl	%eax, %eax
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	addl	%eax, %edx
	movzbl	-44(%ebp), %eax
	movb	%al, (%edx)
	.loc 2 178 0
	movl	-12(%ebp), %eax
	addl	%eax, %eax
	leal	1(%eax), %edx
	movl	-20(%ebp), %eax
	addl	%eax, %edx
	movl	16(%ebp), %eax
	movb	%al, (%edx)
	.loc 2 179 0
	addl	$1, -12(%ebp)
	.loc 2 185 0
	jmp	.L41
.L39:
	.loc 2 183 0
	movl	16(%ebp), %eax
	movl	%eax, -12(%ebp)
	.loc 2 185 0
	jmp	.L41
.L46:
.LBB17:
	.loc 2 189 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	subl	%eax, -12(%ebp)
	.loc 2 190 0
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	movl	%eax, %ebx
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube6HeightEv
	subl	$1, %eax
	addl	%eax, %eax
	imull	%ebx, %eax
	movl	%eax, -24(%ebp)
	.loc 2 191 0
	movl	$0, -16(%ebp)
	jmp	.L42
.L43:
	.loc 2 192 0 discriminator 2
	movl	-16(%ebp), %edx
	movl	-20(%ebp), %eax
	leal	(%edx,%eax), %ebx
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	leal	(%eax,%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movb	%al, (%ebx)
	.loc 2 191 0 discriminator 2
	addl	$1, -16(%ebp)
.L42:
	.loc 2 191 0 is_stmt 0 discriminator 1
	movl	-16(%ebp), %eax
	cmpl	-24(%ebp), %eax
	setl	%al
	testb	%al, %al
	jne	.L43
	.loc 2 193 0 is_stmt 1
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	addl	%eax, %eax
	addl	%eax, -24(%ebp)
	.loc 2 194 0
	jmp	.L44
.L45:
	.loc 2 196 0
	movl	-16(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	$32, (%eax)
	addl	$1, -16(%ebp)
	.loc 2 197 0
	movl	-16(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	$7, (%eax)
	addl	$1, -16(%ebp)
.L44:
	.loc 2 194 0 discriminator 1
	movl	-16(%ebp), %eax
	cmpl	-24(%ebp), %eax
	setl	%al
	testb	%al, %al
	jne	.L45
.L41:
.LBE17:
	.loc 2 185 0 discriminator 1
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5WidthEv
	movl	%eax, %ebx
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube6HeightEv
	imull	%ebx, %eax
	cmpl	-12(%ebp), %eax
	setle	%al
	testb	%al, %al
	jne	.L46
	.loc 2 200 0
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube15SetCursorOffsetEi
	leal	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeD1Ev
.LBE16:
	.loc 2 201 0
	addl	$68, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
.LCFI47:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE26:
	.size	_ZN13ScreenPrinter9PrintCharEci, .-_ZN13ScreenPrinter9PrintCharEci
	.text
	.globl	_Z4kclsv
	.type	_Z4kclsv, @function
_Z4kclsv:
.LFB27:
	.loc 2 205 0
	.cfi_startproc
	pushl	%ebp
.LCFI48:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI49:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB18:
	.loc 2 206 0
	leal	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeC1Ev
	.loc 2 208 0
	leal	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTube5ClearEv
	leal	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN14CathodeRayTubeD1Ev
.LBE18:
	.loc 2 209 0
	leave
.LCFI50:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE27:
	.size	_Z4kclsv, .-_Z4kclsv
	.section	.text._ZN13ScreenPrinterC2Ev,"axG",@progbits,_ZN13ScreenPrinterC5Ev,comdat
	.align 2
	.weak	_ZN13ScreenPrinterC2Ev
	.type	_ZN13ScreenPrinterC2Ev, @function
_ZN13ScreenPrinterC2Ev:
.LFB30:
	.loc 2 116 0
	.cfi_startproc
	pushl	%ebp
.LCFI51:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI52:
	.cfi_def_cfa_register 5
	subl	$24, %esp
.LBB19:
	.loc 2 116 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN12CStringPrintC2Ev
	movl	8(%ebp), %eax
	movl	$_ZTV13ScreenPrinter+8, (%eax)
.LBE19:
	leave
.LCFI53:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE30:
	.size	_ZN13ScreenPrinterC2Ev, .-_ZN13ScreenPrinterC2Ev
	.weak	_ZN13ScreenPrinterC1Ev
	.set	_ZN13ScreenPrinterC1Ev,_ZN13ScreenPrinterC2Ev
	.text
	.globl	_Z7kprintfPKcz
	.type	_Z7kprintfPKcz, @function
_Z7kprintfPKcz:
.LFB28:
	.loc 2 212 0
	.cfi_startproc
	pushl	%ebp
.LCFI54:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI55:
	.cfi_def_cfa_register 5
	subl	$40, %esp
.LBB20:
	.loc 2 213 0
	leal	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinterC1Ev
	.loc 2 216 0
	leal	12(%ebp), %eax
	movl	%eax, -16(%ebp)
	.loc 2 217 0
	leal	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN12CStringPrint9PrintListEPKcRPc
	.loc 2 218 0
	leal	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinterD1Ev
.LBE20:
	.loc 2 219 0
	leave
.LCFI56:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE28:
	.size	_Z7kprintfPKcz, .-_Z7kprintfPKcz
	.weak	_ZTV13ScreenPrinter
	.section	.rodata._ZTV13ScreenPrinter,"aG",@progbits,_ZTV13ScreenPrinter,comdat
	.align 8
	.type	_ZTV13ScreenPrinter, @object
	.size	_ZTV13ScreenPrinter, 20
_ZTV13ScreenPrinter:
	.long	0
	.long	0
	.long	_ZN13ScreenPrinterD1Ev
	.long	_ZN13ScreenPrinterD0Ev
	.long	_ZN13ScreenPrinter8PrintOutEPKci
	.section	.text._ZN13ScreenPrinterD2Ev,"axG",@progbits,_ZN13ScreenPrinterD5Ev,comdat
	.align 2
	.weak	_ZN13ScreenPrinterD2Ev
	.type	_ZN13ScreenPrinterD2Ev, @function
_ZN13ScreenPrinterD2Ev:
.LFB33:
	.loc 2 116 0
	.cfi_startproc
	pushl	%ebp
.LCFI57:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI58:
	.cfi_def_cfa_register 5
	subl	$24, %esp
.LBB21:
	.loc 2 116 0
	movl	8(%ebp), %eax
	movl	$_ZTV13ScreenPrinter+8, (%eax)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN12CStringPrintD2Ev
.LBE21:
	movl	$0, %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L50
	.loc 2 116 0 is_stmt 0 discriminator 2
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZdlPv
.L50:
	.loc 2 116 0
	leave
.LCFI59:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE33:
	.size	_ZN13ScreenPrinterD2Ev, .-_ZN13ScreenPrinterD2Ev
	.weak	_ZN13ScreenPrinterD1Ev
	.set	_ZN13ScreenPrinterD1Ev,_ZN13ScreenPrinterD2Ev
	.section	.text._ZN13ScreenPrinterD0Ev,"axG",@progbits,_ZN13ScreenPrinterD0Ev,comdat
	.align 2
	.weak	_ZN13ScreenPrinterD0Ev
	.type	_ZN13ScreenPrinterD0Ev, @function
_ZN13ScreenPrinterD0Ev:
.LFB35:
	.loc 2 116 0 is_stmt 1
	.cfi_startproc
	pushl	%ebp
.LCFI60:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
.LCFI61:
	.cfi_def_cfa_register 5
	subl	$24, %esp
	.loc 2 116 0
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN13ScreenPrinterD1Ev
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZdlPv
	leave
.LCFI62:
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE35:
	.size	_ZN13ScreenPrinterD0Ev, .-_ZN13ScreenPrinterD0Ev
	.section	.rodata
	.align 8
	.type	_ZL15physicalAddress, @object
	.size	_ZL15physicalAddress, 8
_ZL15physicalAddress:
	.long	753664
	.long	0
	.text
.Letext0:
	.file 3 "/usr/lib/gcc/x86_64-unknown-linux-gnu/4.7.1/include/stdarg.h"
	.file 4 "runtime.h"
	.file 5 "mem_physical.h"
	.file 6 "mem_logical.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0xd8d
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF114
	.byte	0x4
	.long	.LASF115
	.long	.LASF116
	.long	.Ldebug_ranges0+0
	.long	0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF0
	.byte	0x3
	.byte	0x28
	.long	0x34
	.uleb128 0x3
	.byte	0x4
	.long	.LASF117
	.long	0x3e
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.long	.LASF3
	.uleb128 0x2
	.long	.LASF1
	.byte	0x3
	.byte	0x66
	.long	0x29
	.uleb128 0x2
	.long	.LASF2
	.byte	0x4
	.byte	0x8
	.long	0x5b
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF4
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.long	.LASF5
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.long	.LASF6
	.uleb128 0x2
	.long	.LASF7
	.byte	0x4
	.byte	0xc
	.long	0x5b
	.uleb128 0x2
	.long	.LASF8
	.byte	0x5
	.byte	0x6
	.long	0x86
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF9
	.uleb128 0x6
	.byte	0x4
	.byte	0x6
	.byte	0x7
	.long	.LASF13
	.long	0xa8
	.uleb128 0x7
	.long	.LASF10
	.sleb128 0
	.uleb128 0x7
	.long	.LASF11
	.sleb128 1
	.byte	0
	.uleb128 0x2
	.long	.LASF12
	.byte	0x6
	.byte	0xa
	.long	0x8f
	.uleb128 0x6
	.byte	0x4
	.byte	0x6
	.byte	0xc
	.long	.LASF14
	.long	0xd2
	.uleb128 0x7
	.long	.LASF15
	.sleb128 1
	.uleb128 0x7
	.long	.LASF16
	.sleb128 2
	.uleb128 0x7
	.long	.LASF17
	.sleb128 4
	.byte	0
	.uleb128 0x2
	.long	.LASF18
	.byte	0x6
	.byte	0x10
	.long	0xb3
	.uleb128 0x8
	.byte	0x4
	.byte	0x6
	.byte	0x12
	.long	.LASF31
	.long	0x1cf
	.uleb128 0x9
	.byte	0x4
	.byte	0x6
	.byte	0x13
	.long	0x1c6
	.uleb128 0xa
	.byte	0x4
	.byte	0x6
	.byte	0x14
	.long	0x1b5
	.uleb128 0xb
	.long	.LASF19
	.byte	0x6
	.byte	0x15
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF20
	.byte	0x6
	.byte	0x16
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF21
	.byte	0x6
	.byte	0x17
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF22
	.byte	0x6
	.byte	0x18
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF23
	.byte	0x6
	.byte	0x19
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF24
	.byte	0x6
	.byte	0x1a
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF25
	.byte	0x6
	.byte	0x1b
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF26
	.byte	0x6
	.byte	0x1c
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF27
	.byte	0x6
	.byte	0x1d
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF28
	.byte	0x6
	.byte	0x1e
	.long	0x70
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF29
	.byte	0x6
	.byte	0x1f
	.long	0x70
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0xc
	.long	0xf1
	.uleb128 0xd
	.long	.LASF36
	.byte	0x6
	.byte	0x21
	.long	0x70
	.byte	0
	.uleb128 0xe
	.long	0xe9
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF30
	.byte	0x6
	.byte	0x23
	.long	0xdd
	.uleb128 0x8
	.byte	0x4
	.byte	0x6
	.byte	0x25
	.long	.LASF32
	.long	0x2cc
	.uleb128 0x9
	.byte	0x4
	.byte	0x6
	.byte	0x26
	.long	0x2c3
	.uleb128 0xa
	.byte	0x4
	.byte	0x6
	.byte	0x27
	.long	0x2b2
	.uleb128 0xb
	.long	.LASF19
	.byte	0x6
	.byte	0x28
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF20
	.byte	0x6
	.byte	0x29
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF21
	.byte	0x6
	.byte	0x2a
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF33
	.byte	0x6
	.byte	0x2b
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF23
	.byte	0x6
	.byte	0x2c
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1b
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF24
	.byte	0x6
	.byte	0x2d
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x1a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF34
	.byte	0x6
	.byte	0x2e
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x19
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF35
	.byte	0x6
	.byte	0x2f
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF27
	.byte	0x6
	.byte	0x30
	.long	0x70
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF28
	.byte	0x6
	.byte	0x31
	.long	0x70
	.byte	0x4
	.byte	0x3
	.byte	0x14
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.long	.LASF29
	.byte	0x6
	.byte	0x32
	.long	0x70
	.byte	0x4
	.byte	0x14
	.byte	0
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0xc
	.long	0x1ee
	.uleb128 0xd
	.long	.LASF36
	.byte	0x6
	.byte	0x34
	.long	0x70
	.byte	0
	.uleb128 0xe
	.long	0x1e6
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x2
	.long	.LASF37
	.byte	0x6
	.byte	0x36
	.long	0x1da
	.uleb128 0xf
	.value	0x1000
	.byte	0x6
	.byte	0x38
	.long	.LASF118
	.long	0x2f3
	.uleb128 0x10
	.long	.LASF40
	.byte	0x6
	.byte	0x39
	.long	0x2f3
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0
	.uleb128 0x11
	.long	0x2cc
	.long	0x304
	.uleb128 0x12
	.long	0x304
	.value	0x3ff
	.byte	0
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF38
	.uleb128 0x2
	.long	.LASF39
	.byte	0x6
	.byte	0x3a
	.long	0x2d7
	.uleb128 0x13
	.long	.LASF60
	.value	0x100c
	.byte	0x6
	.byte	0x3c
	.long	0x4b5
	.uleb128 0x10
	.long	.LASF41
	.byte	0x6
	.byte	0x40
	.long	0x4b5
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x10
	.long	.LASF42
	.byte	0x6
	.byte	0x41
	.long	0x4cc
	.byte	0x3
	.byte	0x23
	.uleb128 0x1000
	.uleb128 0x10
	.long	.LASF43
	.byte	0x6
	.byte	0x42
	.long	0x7b
	.byte	0x3
	.byte	0x23
	.uleb128 0x1004
	.uleb128 0x14
	.long	.LASF62
	.byte	0x6
	.byte	0x53
	.long	0x4d2
	.byte	0x3
	.byte	0x23
	.uleb128 0x1008
	.byte	0x3
	.uleb128 0x15
	.byte	0x1
	.long	.LASF44
	.byte	0x6
	.byte	0x44
	.long	.LASF46
	.byte	0x1
	.long	0x374
	.long	0x37b
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF45
	.byte	0x6
	.byte	0x46
	.long	.LASF47
	.byte	0x1
	.long	0x390
	.long	0x397
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF48
	.byte	0x6
	.byte	0x47
	.long	.LASF49
	.byte	0x1
	.long	0x3ac
	.long	0x3b3
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF50
	.byte	0x6
	.byte	0x48
	.long	.LASF51
	.byte	0x1
	.long	0x3c8
	.long	0x3cf
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.byte	0
	.uleb128 0x17
	.byte	0x1
	.string	"Map"
	.byte	0x6
	.byte	0x4a
	.long	.LASF119
	.long	0x86
	.byte	0x1
	.long	0x3e8
	.long	0x403
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.uleb128 0x18
	.long	0x4d2
	.uleb128 0x18
	.long	0xa8
	.uleb128 0x18
	.long	0x7b
	.uleb128 0x18
	.long	0x50
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.string	"Map"
	.byte	0x6
	.byte	0x4b
	.long	.LASF120
	.byte	0x1
	.long	0x418
	.long	0x42e
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.uleb128 0x18
	.long	0x4d2
	.uleb128 0x18
	.long	0x86
	.uleb128 0x18
	.long	0x7b
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF52
	.byte	0x6
	.byte	0x4c
	.long	.LASF53
	.byte	0x1
	.long	0x443
	.long	0x44f
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.uleb128 0x18
	.long	0x86
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF54
	.byte	0x6
	.byte	0x4d
	.long	.LASF55
	.long	0x7b
	.byte	0x1
	.long	0x468
	.long	0x474
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.uleb128 0x18
	.long	0x86
	.byte	0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF52
	.byte	0x6
	.byte	0x50
	.long	.LASF56
	.byte	0x3
	.byte	0x1
	.long	0x48a
	.long	0x49b
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.uleb128 0x18
	.long	0x86
	.uleb128 0x18
	.long	0x4df
	.byte	0
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF57
	.byte	0x6
	.byte	0x51
	.long	.LASF58
	.byte	0x3
	.byte	0x1
	.long	0x4ad
	.uleb128 0x16
	.long	0x4d9
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x11
	.long	0x4c6
	.long	0x4c6
	.uleb128 0x12
	.long	0x304
	.value	0x3ff
	.byte	0
	.uleb128 0x1d
	.byte	0x4
	.long	0x30b
	.uleb128 0x1d
	.byte	0x4
	.long	0x1cf
	.uleb128 0x1e
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x1d
	.byte	0x4
	.long	0x316
	.uleb128 0x4
	.byte	0x1
	.byte	0x2
	.long	.LASF59
	.uleb128 0x1f
	.long	.LASF61
	.byte	0x8
	.byte	0x2
	.byte	0x22
	.long	0x655
	.uleb128 0x20
	.string	"set"
	.byte	0x2
	.byte	0x25
	.long	0x4df
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x3
	.uleb128 0x20
	.string	"got"
	.byte	0x2
	.byte	0x26
	.long	0x4d2
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x3
	.uleb128 0x21
	.byte	0x1
	.long	.LASF61
	.byte	0x2
	.byte	0x28
	.byte	0x1
	.long	0x521
	.long	0x528
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.uleb128 0x21
	.byte	0x1
	.long	.LASF63
	.byte	0x2
	.byte	0x2c
	.byte	0x1
	.long	0x539
	.long	0x546
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.uleb128 0x16
	.long	0x4d2
	.byte	0x1
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF64
	.byte	0x2
	.byte	0x31
	.long	.LASF65
	.long	0x4d2
	.byte	0x1
	.long	0x55f
	.long	0x566
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF66
	.byte	0x2
	.byte	0x35
	.long	.LASF67
	.long	0x4d2
	.byte	0x1
	.long	0x57f
	.long	0x586
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF68
	.byte	0x2
	.byte	0x3b
	.long	.LASF69
	.long	0x65b
	.byte	0x1
	.long	0x59f
	.long	0x5a6
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF70
	.byte	0x2
	.byte	0x41
	.long	.LASF71
	.long	0x4d2
	.byte	0x1
	.long	0x5bf
	.long	0x5c6
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF72
	.byte	0x2
	.byte	0x51
	.long	.LASF73
	.byte	0x1
	.long	0x5db
	.long	0x5e7
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.uleb128 0x18
	.long	0x4d2
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF74
	.byte	0x2
	.byte	0x5c
	.long	.LASF75
	.byte	0x1
	.long	0x5fc
	.long	0x612
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.uleb128 0x18
	.long	0x4d2
	.uleb128 0x18
	.long	0x661
	.uleb128 0x18
	.long	0x661
	.byte	0
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF76
	.byte	0x2
	.byte	0x63
	.long	.LASF77
	.long	0x4d2
	.byte	0x1
	.long	0x62b
	.long	0x63c
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.uleb128 0x18
	.long	0x4d2
	.uleb128 0x18
	.long	0x4d2
	.byte	0
	.uleb128 0x22
	.byte	0x1
	.long	.LASF78
	.byte	0x2
	.byte	0x69
	.long	.LASF121
	.byte	0x1
	.long	0x64d
	.uleb128 0x16
	.long	0x655
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x1d
	.byte	0x4
	.long	0x4e6
	.uleb128 0x1d
	.byte	0x4
	.long	0x3e
	.uleb128 0x1d
	.byte	0x4
	.long	0x4d2
	.uleb128 0x23
	.long	.LASF79
	.byte	0x4
	.byte	0x2
	.byte	0x74
	.long	0x74a
	.long	0x74a
	.uleb128 0x24
	.long	0x74a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x1
	.uleb128 0x25
	.byte	0x1
	.long	.LASF79
	.byte	0x1
	.byte	0x1
	.long	0x690
	.long	0x69c
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.uleb128 0x18
	.long	0x756
	.byte	0
	.uleb128 0x25
	.byte	0x1
	.long	.LASF79
	.byte	0x1
	.byte	0x1
	.long	0x6ac
	.long	0x6b3
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF122
	.byte	0x2
	.byte	0x77
	.long	.LASF123
	.byte	0x1
	.byte	0x2
	.byte	0x10
	.uleb128 0x2
	.long	0x667
	.byte	0x2
	.byte	0x1
	.long	0x6d1
	.long	0x6e2
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.uleb128 0x18
	.long	0x761
	.uleb128 0x18
	.long	0x4d2
	.byte	0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF80
	.byte	0x2
	.byte	0x81
	.long	.LASF81
	.byte	0x3
	.byte	0x1
	.long	0x6f8
	.long	0x704
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.uleb128 0x18
	.long	0x3e
	.byte	0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF82
	.byte	0x2
	.byte	0xa7
	.long	.LASF83
	.byte	0x3
	.byte	0x1
	.long	0x71a
	.long	0x72b
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.uleb128 0x18
	.long	0x3e
	.uleb128 0x18
	.long	0x4d2
	.byte	0
	.uleb128 0x27
	.byte	0x1
	.long	.LASF84
	.byte	0x1
	.long	0x667
	.byte	0x1
	.byte	0x1
	.long	0x73c
	.uleb128 0x16
	.long	0x750
	.byte	0x1
	.uleb128 0x16
	.long	0x4d2
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x28
	.long	.LASF124
	.byte	0x1
	.uleb128 0x1d
	.byte	0x4
	.long	0x667
	.uleb128 0x29
	.byte	0x4
	.long	0x75c
	.uleb128 0x2a
	.long	0x667
	.uleb128 0x1d
	.byte	0x4
	.long	0x767
	.uleb128 0x2a
	.long	0x3e
	.uleb128 0x2b
	.long	.LASF125
	.byte	0x1
	.byte	0x20
	.long	.LFB0
	.long	.LFE0
	.long	.LLST0
	.byte	0x1
	.long	0x7a1
	.uleb128 0x2c
	.long	.LASF85
	.byte	0x1
	.byte	0x20
	.long	0x69
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x2d
	.string	"val"
	.byte	0x1
	.byte	0x20
	.long	0x62
	.byte	0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x2e
	.string	"inb"
	.byte	0x1
	.byte	0x2f
	.long	0x62
	.long	.LFB3
	.long	.LFE3
	.long	.LLST1
	.byte	0x1
	.long	0x7e4
	.uleb128 0x2c
	.long	.LASF85
	.byte	0x1
	.byte	0x2f
	.long	0x69
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2f
	.long	.LBB2
	.long	.LBE2
	.uleb128 0x30
	.string	"ret"
	.byte	0x1
	.byte	0x31
	.long	0x62
	.byte	0x2
	.byte	0x91
	.sleb128 -13
	.byte	0
	.byte	0
	.uleb128 0x31
	.long	.LASF126
	.byte	0x2
	.byte	0xc
	.long	0x65b
	.long	.LFB7
	.long	.LFE7
	.long	.LLST2
	.byte	0x1
	.uleb128 0x32
	.byte	0x1
	.long	.LASF127
	.byte	0x2
	.byte	0x14
	.long	.LASF128
	.long	.LFB8
	.long	.LFE8
	.long	.LLST3
	.byte	0x1
	.uleb128 0x33
	.byte	0x1
	.long	.LASF97
	.byte	0x2
	.byte	0x1b
	.long	.LASF99
	.long	.LFB9
	.long	.LFE9
	.long	.LLST4
	.byte	0x1
	.long	0x857
	.uleb128 0x2d
	.string	"c"
	.byte	0x2
	.byte	0x1b
	.long	0x3e
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2f
	.long	.LBB3
	.long	.LBE3
	.uleb128 0x34
	.long	.LASF86
	.byte	0x2
	.byte	0x1d
	.long	0x65b
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.byte	0
	.uleb128 0x35
	.long	0x510
	.byte	0x2
	.long	0x865
	.long	0x870
	.uleb128 0x36
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0
	.uleb128 0x2a
	.long	0x655
	.uleb128 0x37
	.long	0x857
	.long	.LASF89
	.long	.LFB11
	.long	.LFE11
	.long	.LLST5
	.long	0x893
	.byte	0x1
	.long	0x89c
	.uleb128 0x38
	.long	0x865
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x35
	.long	0x528
	.byte	0x2
	.long	0x8aa
	.long	0x8bf
	.uleb128 0x36
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.uleb128 0x36
	.long	.LASF88
	.long	0x8bf
	.byte	0x1
	.byte	0
	.uleb128 0x2a
	.long	0x4d2
	.uleb128 0x37
	.long	0x89c
	.long	.LASF90
	.long	.LFB14
	.long	.LFE14
	.long	.LLST6
	.long	0x8e2
	.byte	0x1
	.long	0x8eb
	.uleb128 0x38
	.long	0x8aa
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x39
	.long	0x546
	.long	.LFB16
	.long	.LFE16
	.long	.LLST7
	.long	0x905
	.byte	0x1
	.long	0x913
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x39
	.long	0x566
	.long	.LFB17
	.long	.LFE17
	.long	.LLST8
	.long	0x92d
	.byte	0x1
	.long	0x93b
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x3b
	.long	0x586
	.long	.LFB18
	.long	.LFE18
	.long	.LLST9
	.long	0x955
	.byte	0x1
	.long	0x963
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x3b
	.long	0x5a6
	.long	.LFB19
	.long	.LFE19
	.long	.LLST10
	.long	0x97d
	.byte	0x1
	.long	0x9b1
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2f
	.long	.LBB5
	.long	.LBE5
	.uleb128 0x30
	.string	"low"
	.byte	0x2
	.byte	0x43
	.long	0x62
	.byte	0x2
	.byte	0x91
	.sleb128 -10
	.uleb128 0x34
	.long	.LASF91
	.byte	0x2
	.byte	0x43
	.long	0x62
	.byte	0x2
	.byte	0x91
	.sleb128 -9
	.byte	0
	.byte	0
	.uleb128 0x3b
	.long	0x5c6
	.long	.LFB20
	.long	.LFE20
	.long	.LLST11
	.long	0x9cb
	.byte	0x1
	.long	0x9e7
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2c
	.long	.LASF92
	.byte	0x2
	.byte	0x51
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.byte	0
	.uleb128 0x3b
	.long	0x63c
	.long	.LFB23
	.long	.LFE23
	.long	.LLST12
	.long	0xa01
	.byte	0x1
	.long	0xa33
	.uleb128 0x3a
	.long	.LASF87
	.long	0x870
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2f
	.long	.LBB6
	.long	.LBE6
	.uleb128 0x34
	.long	.LASF93
	.byte	0x2
	.byte	0x6b
	.long	0xa33
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x30
	.string	"i"
	.byte	0x2
	.byte	0x6c
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x1d
	.byte	0x4
	.long	0x69
	.uleb128 0x3b
	.long	0x6b3
	.long	.LFB24
	.long	.LFE24
	.long	.LLST13
	.long	0xa53
	.byte	0x1
	.long	0xa93
	.uleb128 0x3a
	.long	.LASF87
	.long	0xa93
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2c
	.long	.LASF93
	.byte	0x2
	.byte	0x77
	.long	0x761
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x2c
	.long	.LASF94
	.byte	0x2
	.byte	0x77
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x2f
	.long	.LBB8
	.long	.LBE8
	.uleb128 0x30
	.string	"i"
	.byte	0x2
	.byte	0x79
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x2a
	.long	0x750
	.uleb128 0x3b
	.long	0x6e2
	.long	.LFB25
	.long	.LFE25
	.long	.LLST14
	.long	0xab2
	.byte	0x1
	.long	0xb61
	.uleb128 0x3a
	.long	.LASF87
	.long	0xa93
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2d
	.string	"c"
	.byte	0x2
	.byte	0x81
	.long	0x3e
	.byte	0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x3c
	.long	.LBB12
	.long	.LBE12
	.long	0xae6
	.uleb128 0x30
	.string	"i"
	.byte	0x2
	.byte	0x86
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x3c
	.long	.LBB13
	.long	.LBE13
	.long	0xb10
	.uleb128 0x34
	.long	.LASF95
	.byte	0x2
	.byte	0x8b
	.long	0x4e6
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x34
	.long	.LASF96
	.byte	0x2
	.byte	0x8c
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x3c
	.long	.LBB14
	.long	.LBE14
	.long	0xb3a
	.uleb128 0x34
	.long	.LASF95
	.byte	0x2
	.byte	0x93
	.long	0x4e6
	.byte	0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x34
	.long	.LASF96
	.byte	0x2
	.byte	0x94
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.uleb128 0x2f
	.long	.LBB15
	.long	.LBE15
	.uleb128 0x34
	.long	.LASF95
	.byte	0x2
	.byte	0x9b
	.long	0x4e6
	.byte	0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x34
	.long	.LASF96
	.byte	0x2
	.byte	0x9c
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.byte	0
	.uleb128 0x3b
	.long	0x704
	.long	.LFB26
	.long	.LFE26
	.long	.LLST15
	.long	0xb7b
	.byte	0x1
	.long	0xbf9
	.uleb128 0x3a
	.long	.LASF87
	.long	0xa93
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2d
	.string	"c"
	.byte	0x2
	.byte	0xa7
	.long	0x3e
	.byte	0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x2d
	.string	"f"
	.byte	0x2
	.byte	0xa7
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x2f
	.long	.LBB16
	.long	.LBE16
	.uleb128 0x34
	.long	.LASF95
	.byte	0x2
	.byte	0xa9
	.long	0x4e6
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x34
	.long	.LASF93
	.byte	0x2
	.byte	0xaa
	.long	0x65b
	.byte	0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x34
	.long	.LASF96
	.byte	0x2
	.byte	0xab
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2f
	.long	.LBB17
	.long	.LBE17
	.uleb128 0x30
	.string	"i"
	.byte	0x2
	.byte	0xbb
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x30
	.string	"len"
	.byte	0x2
	.byte	0xbb
	.long	0x4d2
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x33
	.byte	0x1
	.long	.LASF98
	.byte	0x2
	.byte	0xcc
	.long	.LASF100
	.long	.LFB27
	.long	.LFE27
	.long	.LLST16
	.byte	0x1
	.long	0xc2f
	.uleb128 0x2f
	.long	.LBB18
	.long	.LBE18
	.uleb128 0x30
	.string	"crt"
	.byte	0x2
	.byte	0xce
	.long	0x4e6
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x3d
	.long	0x69c
	.byte	0x2
	.byte	0x74
	.byte	0x2
	.long	0xc3f
	.long	0xc4a
	.uleb128 0x36
	.long	.LASF87
	.long	0xa93
	.byte	0x1
	.byte	0
	.uleb128 0x3e
	.long	0xc2f
	.long	.LASF101
	.long	.LFB30
	.long	.LFE30
	.long	.LLST17
	.long	0xc68
	.byte	0x1
	.long	0xc71
	.uleb128 0x38
	.long	0xc3f
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x33
	.byte	0x1
	.long	.LASF102
	.byte	0x2
	.byte	0xd3
	.long	.LASF103
	.long	.LFB28
	.long	.LFE28
	.long	.LLST18
	.byte	0x1
	.long	0xcc3
	.uleb128 0x2c
	.long	.LASF104
	.byte	0x2
	.byte	0xd3
	.long	0x761
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x3f
	.uleb128 0x2f
	.long	.LBB20
	.long	.LBE20
	.uleb128 0x34
	.long	.LASF105
	.byte	0x2
	.byte	0xd5
	.long	0x667
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x30
	.string	"vl"
	.byte	0x2
	.byte	0xd6
	.long	0x45
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x3d
	.long	0x72b
	.byte	0x2
	.byte	0x74
	.byte	0x2
	.long	0xcd3
	.long	0xce8
	.uleb128 0x36
	.long	.LASF87
	.long	0xa93
	.byte	0x1
	.uleb128 0x36
	.long	.LASF88
	.long	0xce8
	.byte	0x1
	.byte	0
	.uleb128 0x2a
	.long	0x4d2
	.uleb128 0x3e
	.long	0xcc3
	.long	.LASF106
	.long	.LFB33
	.long	.LFE33
	.long	.LLST19
	.long	0xd0b
	.byte	0x1
	.long	0xd14
	.uleb128 0x38
	.long	0xcd3
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x3e
	.long	0xcc3
	.long	.LASF107
	.long	.LFB35
	.long	.LFE35
	.long	.LLST20
	.long	0xd32
	.byte	0x1
	.long	0xd3b
	.uleb128 0x38
	.long	0xcd3
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x40
	.long	.LASF108
	.byte	0x6
	.byte	0x56
	.long	0x316
	.byte	0x1
	.byte	0x1
	.uleb128 0x40
	.long	.LASF109
	.byte	0x2
	.byte	0x7
	.long	0xd55
	.byte	0x1
	.byte	0x1
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.long	.LASF110
	.uleb128 0x40
	.long	.LASF111
	.byte	0x2
	.byte	0x7
	.long	0xd55
	.byte	0x1
	.byte	0x1
	.uleb128 0x34
	.long	.LASF112
	.byte	0x2
	.byte	0x8
	.long	0xd7a
	.byte	0x5
	.byte	0x3
	.long	_ZL15physicalAddress
	.uleb128 0x2a
	.long	0xd55
	.uleb128 0x34
	.long	.LASF113
	.byte	0x2
	.byte	0xa
	.long	0x65b
	.byte	0x5
	.byte	0x3
	.long	_ZL13s_framebuffer
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
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
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
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
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
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
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
	.uleb128 0x7
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xb
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
	.uleb128 0xc
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
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
	.uleb128 0xe
	.uleb128 0xd
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xf
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
	.uleb128 0x10
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
	.uleb128 0x11
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x13
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
	.uleb128 0x14
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
	.uleb128 0x15
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
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x17
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
	.uleb128 0x18
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
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
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
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
	.uleb128 0x1f
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
	.uleb128 0x20
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
	.uleb128 0x32
	.uleb128 0xb
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
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
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
	.uleb128 0x1d
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x1c
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x32
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x34
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
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
	.uleb128 0x4c
	.uleb128 0xb
	.uleb128 0x4d
	.uleb128 0xa
	.uleb128 0x1d
	.uleb128 0x13
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
	.uleb128 0x27
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x4c
	.uleb128 0xb
	.uleb128 0x1d
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x2
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x10
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2b
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
	.uleb128 0x2c
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
	.uleb128 0x2d
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
	.uleb128 0x2e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x30
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
	.uleb128 0x31
	.uleb128 0x2e
	.byte	0
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
	.byte	0
	.byte	0
	.uleb128 0x32
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
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x33
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
	.uleb128 0x34
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
	.uleb128 0x35
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x36
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x37
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
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
	.uleb128 0x38
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x39
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
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
	.uleb128 0x3a
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
	.uleb128 0x3b
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
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
	.uleb128 0x3c
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
	.uleb128 0x3d
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
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
	.uleb128 0x3f
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x40
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
	.long	.LFB0
	.long	.LCFI0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI0
	.long	.LCFI1
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI1
	.long	.LCFI2
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI2
	.long	.LFE0
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST1:
	.long	.LFB3
	.long	.LCFI3
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI3
	.long	.LCFI4
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI4
	.long	.LCFI5
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI5
	.long	.LFE3
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST2:
	.long	.LFB7
	.long	.LCFI6
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI6
	.long	.LCFI7
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI7
	.long	.LCFI8
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI8
	.long	.LFE7
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST3:
	.long	.LFB8
	.long	.LCFI9
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI9
	.long	.LCFI10
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI10
	.long	.LCFI11
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI11
	.long	.LFE8
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST4:
	.long	.LFB9
	.long	.LCFI12
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI12
	.long	.LCFI13
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI13
	.long	.LCFI14
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI14
	.long	.LFE9
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST5:
	.long	.LFB11
	.long	.LCFI15
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI15
	.long	.LCFI16
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI16
	.long	.LCFI17
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI17
	.long	.LFE11
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST6:
	.long	.LFB14
	.long	.LCFI18
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI18
	.long	.LCFI19
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI19
	.long	.LCFI20
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI20
	.long	.LFE14
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST7:
	.long	.LFB16
	.long	.LCFI21
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI21
	.long	.LCFI22
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI22
	.long	.LCFI23
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI23
	.long	.LFE16
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST8:
	.long	.LFB17
	.long	.LCFI24
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI24
	.long	.LCFI25
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI25
	.long	.LCFI26
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI26
	.long	.LFE17
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST9:
	.long	.LFB18
	.long	.LCFI27
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI27
	.long	.LCFI28
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI28
	.long	.LCFI29
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI29
	.long	.LFE18
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST10:
	.long	.LFB19
	.long	.LCFI30
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI30
	.long	.LCFI31
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI31
	.long	.LCFI32
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI32
	.long	.LFE19
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST11:
	.long	.LFB20
	.long	.LCFI33
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI33
	.long	.LCFI34
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI34
	.long	.LCFI35
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI35
	.long	.LFE20
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST12:
	.long	.LFB23
	.long	.LCFI36
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI36
	.long	.LCFI37
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI37
	.long	.LCFI38
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI38
	.long	.LFE23
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST13:
	.long	.LFB24
	.long	.LCFI39
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI39
	.long	.LCFI40
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI40
	.long	.LCFI41
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI41
	.long	.LFE24
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST14:
	.long	.LFB25
	.long	.LCFI42
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI42
	.long	.LCFI43
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI43
	.long	.LCFI44
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI44
	.long	.LFE25
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST15:
	.long	.LFB26
	.long	.LCFI45
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI45
	.long	.LCFI46
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI46
	.long	.LCFI47
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI47
	.long	.LFE26
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST16:
	.long	.LFB27
	.long	.LCFI48
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI48
	.long	.LCFI49
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI49
	.long	.LCFI50
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI50
	.long	.LFE27
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST17:
	.long	.LFB30
	.long	.LCFI51
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI51
	.long	.LCFI52
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI52
	.long	.LCFI53
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI53
	.long	.LFE30
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST18:
	.long	.LFB28
	.long	.LCFI54
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI54
	.long	.LCFI55
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI55
	.long	.LCFI56
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI56
	.long	.LFE28
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST19:
	.long	.LFB33
	.long	.LCFI57
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI57
	.long	.LCFI58
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI58
	.long	.LCFI59
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI59
	.long	.LFE33
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
.LLST20:
	.long	.LFB35
	.long	.LCFI60
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	.LCFI60
	.long	.LCFI61
	.value	0x2
	.byte	0x74
	.sleb128 8
	.long	.LCFI61
	.long	.LCFI62
	.value	0x2
	.byte	0x75
	.sleb128 8
	.long	.LCFI62
	.long	.LFE35
	.value	0x2
	.byte	0x74
	.sleb128 4
	.long	0
	.long	0
	.section	.debug_aranges,"",@progbits
	.long	0x8c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.LFB11
	.long	.LFE11-.LFB11
	.long	.LFB14
	.long	.LFE14-.LFB14
	.long	.LFB16
	.long	.LFE16-.LFB16
	.long	.LFB17
	.long	.LFE17-.LFB17
	.long	.LFB18
	.long	.LFE18-.LFB18
	.long	.LFB19
	.long	.LFE19-.LFB19
	.long	.LFB20
	.long	.LFE20-.LFB20
	.long	.LFB23
	.long	.LFE23-.LFB23
	.long	.LFB24
	.long	.LFE24-.LFB24
	.long	.LFB25
	.long	.LFE25-.LFB25
	.long	.LFB26
	.long	.LFE26-.LFB26
	.long	.LFB30
	.long	.LFE30-.LFB30
	.long	.LFB33
	.long	.LFE33-.LFB33
	.long	.LFB35
	.long	.LFE35-.LFB35
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.Ltext0
	.long	.Letext0
	.long	.LFB11
	.long	.LFE11
	.long	.LFB14
	.long	.LFE14
	.long	.LFB16
	.long	.LFE16
	.long	.LFB17
	.long	.LFE17
	.long	.LFB18
	.long	.LFE18
	.long	.LFB19
	.long	.LFE19
	.long	.LFB20
	.long	.LFE20
	.long	.LFB23
	.long	.LFE23
	.long	.LFB24
	.long	.LFE24
	.long	.LFB25
	.long	.LFE25
	.long	.LFB26
	.long	.LFE26
	.long	.LFB30
	.long	.LFE30
	.long	.LFB33
	.long	.LFE33
	.long	.LFB35
	.long	.LFE35
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF33:
	.string	"WriteThough"
.LASF71:
	.string	"_ZN14CathodeRayTube15GetCursorOffsetEv"
.LASF77:
	.string	"_ZN14CathodeRayTube10XyToOffsetEii"
.LASF72:
	.string	"SetCursorOffset"
.LASF127:
	.string	"InitDebug"
.LASF98:
	.string	"kcls"
.LASF80:
	.string	"HandleChar"
.LASF119:
	.string	"_ZN18SPageDirectoryInfo3MapEi8MAP_TYPEPvj"
.LASF61:
	.string	"CathodeRayTube"
.LASF111:
	.string	"phys"
.LASF52:
	.string	"Unmap"
.LASF102:
	.string	"kprintf"
.LASF47:
	.string	"_ZN18SPageDirectoryInfo4InitEv"
.LASF2:
	.string	"size_t"
.LASF36:
	.string	"RawValue"
.LASF38:
	.string	"sizetype"
.LASF79:
	.string	"ScreenPrinter"
.LASF43:
	.string	"directoryPhysical"
.LASF44:
	.string	"InitKernel"
.LASF94:
	.string	"length"
.LASF121:
	.string	"_ZN14CathodeRayTube5ClearEv"
.LASF41:
	.string	"tables"
.LASF100:
	.string	"_Z4kclsv"
.LASF25:
	.string	"Available"
.LASF93:
	.string	"data"
.LASF54:
	.string	"Address"
.LASF1:
	.string	"va_list"
.LASF78:
	.string	"Clear"
.LASF17:
	.string	"fmPageData"
.LASF70:
	.string	"GetCursorOffset"
.LASF99:
	.string	"_Z4testc"
.LASF112:
	.string	"physicalAddress"
.LASF67:
	.string	"_ZN14CathodeRayTube6HeightEv"
.LASF13:
	.string	"8MAP_TYPE"
.LASF82:
	.string	"PrintChar"
.LASF84:
	.string	"~ScreenPrinter"
.LASF95:
	.string	"tube"
.LASF12:
	.string	"MAP_TYPE"
.LASF59:
	.string	"bool"
.LASF22:
	.string	"WriteThrough"
.LASF120:
	.string	"_ZN18SPageDirectoryInfo3MapEiPvS0_"
.LASF28:
	.string	"UserData"
.LASF16:
	.string	"fmUser"
.LASF104:
	.string	"format"
.LASF56:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPvb"
.LASF101:
	.string	"_ZN13ScreenPrinterC2Ev"
.LASF108:
	.string	"rootAddressSpace"
.LASF45:
	.string	"Init"
.LASF34:
	.string	"Dirty"
.LASF76:
	.string	"XyToOffset"
.LASF10:
	.string	"pmApplication"
.LASF91:
	.string	"high"
.LASF62:
	.string	"revision"
.LASF50:
	.string	"Select"
.LASF29:
	.string	"Frame"
.LASF11:
	.string	"pmKernel"
.LASF114:
	.string	"GNU C++ 4.7.1 20120721 (prerelease)"
.LASF46:
	.string	"_ZN18SPageDirectoryInfo10InitKernelEv"
.LASF42:
	.string	"tablesPhysical"
.LASF27:
	.string	"Global"
.LASF23:
	.string	"CacheDisable"
.LASF32:
	.string	"5SPage"
.LASF35:
	.string	"PatIndex"
.LASF116:
	.string	"/home/colin/Development/OSdevelopment/Kernel/Kernel/temp"
.LASF122:
	.string	"PrintOut"
.LASF5:
	.string	"unsigned char"
.LASF96:
	.string	"cursor"
.LASF31:
	.string	"14SPageDirectory"
.LASF19:
	.string	"Present"
.LASF68:
	.string	"FrameBuffer"
.LASF126:
	.string	"Get_Framebuffer"
.LASF4:
	.string	"unsigned int"
.LASF37:
	.string	"SPage"
.LASF90:
	.string	"_ZN14CathodeRayTubeD2Ev"
.LASF64:
	.string	"Width"
.LASF118:
	.string	"10SPageTable"
.LASF124:
	.string	"CStringPrint"
.LASF49:
	.string	"_ZN18SPageDirectoryInfo7ReleaseEv"
.LASF123:
	.string	"_ZN13ScreenPrinter8PrintOutEPKci"
.LASF106:
	.string	"_ZN13ScreenPrinterD2Ev"
.LASF6:
	.string	"short unsigned int"
.LASF92:
	.string	"offset"
.LASF58:
	.string	"_ZN18SPageDirectoryInfo10CopyKernelEv"
.LASF3:
	.string	"char"
.LASF89:
	.string	"_ZN14CathodeRayTubeC2Ev"
.LASF86:
	.string	"output"
.LASF88:
	.string	"__in_chrg"
.LASF75:
	.string	"_ZN14CathodeRayTube10OffsetToXyEiPiS0_"
.LASF87:
	.string	"this"
.LASF85:
	.string	"port"
.LASF97:
	.string	"test"
.LASF128:
	.string	"_Z9InitDebugv"
.LASF53:
	.string	"_ZN18SPageDirectoryInfo5UnmapEPv"
.LASF115:
	.string	"debug.cpp"
.LASF48:
	.string	"Release"
.LASF7:
	.string	"UInt32"
.LASF81:
	.string	"_ZN13ScreenPrinter10HandleCharEc"
.LASF9:
	.string	"long unsigned int"
.LASF117:
	.string	"__builtin_va_list"
.LASF66:
	.string	"Height"
.LASF55:
	.string	"_ZN18SPageDirectoryInfo7AddressEPv"
.LASF69:
	.string	"_ZN14CathodeRayTube11FrameBufferEv"
.LASF113:
	.string	"s_framebuffer"
.LASF110:
	.string	"long long int"
.LASF21:
	.string	"User"
.LASF24:
	.string	"Accessed"
.LASF105:
	.string	"printer"
.LASF30:
	.string	"SPageDirectory"
.LASF107:
	.string	"_ZN13ScreenPrinterD0Ev"
.LASF26:
	.string	"PageSize"
.LASF65:
	.string	"_ZN14CathodeRayTube5WidthEv"
.LASF15:
	.string	"fmWritable"
.LASF51:
	.string	"_ZN18SPageDirectoryInfo6SelectEv"
.LASF0:
	.string	"__gnuc_va_list"
.LASF57:
	.string	"CopyKernel"
.LASF63:
	.string	"~CathodeRayTube"
.LASF73:
	.string	"_ZN14CathodeRayTube15SetCursorOffsetEi"
.LASF20:
	.string	"Writable"
.LASF60:
	.string	"SPageDirectoryInfo"
.LASF103:
	.string	"_Z7kprintfPKcz"
.LASF8:
	.string	"PhysicalPointer"
.LASF83:
	.string	"_ZN13ScreenPrinter9PrintCharEci"
.LASF40:
	.string	"pages"
.LASF74:
	.string	"OffsetToXy"
.LASF39:
	.string	"SPageTable"
.LASF109:
	.string	"virt"
.LASF18:
	.string	"MAP_FLAGS"
.LASF125:
	.string	"outb"
.LASF14:
	.string	"9MAP_FLAGS"
	.ident	"GCC: (GNU) 4.7.1 20120721 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
