hang:
    wfi
    b     hang

.globl enable_irq
enable_irq:
    msr   daifclr, #2
    ret

.globl disable_irq
disable_irq:
    msr   daifset, #2
    ret

_exception_entry:
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	str x0, [sp, #-16]!
	mrs	x1, elr_el1
	mrs	x2, spsr_el1
	stp	x1, x2, [sp, #-16]!
    ret

_exception_exit:
	ldp	x1, x2, [sp],#16
	msr	elr_el1, x1
	msr	spsr_el1, x2
    ldr	x0, [sp],#16
	ldp	x1, x2, [sp],#16
	ldp	x3, x4, [sp],#16
	ldp	x5, x6, [sp],#16
	ldp	x7, x8, [sp],#16
	ldp	x9, x10, [sp],#16
	ldp	x11, x12, [sp],#16
	ldp	x13, x14, [sp],#16
	ldp	x15, x16, [sp],#16
	ldp	x17, x18, [sp],#16
	ldp	x19, x20, [sp],#16
	ldp	x21, x22, [sp],#16
	ldp	x23, x24, [sp],#16
	ldp	x25, x26, [sp],#16
	ldp	x27, x28, [sp],#16
	ldp	x29, x30, [sp],#16
	eret

    .align	11
	.globl	vectors
vectors:
    .balign 128
        b hang
    .balign 128
        msr SPSel, #0
        bl _exception_entry
        msr SPSel, #1
        msr daifclr, #0b0010
	    bl	c_process_irq_handler
        MSR daifset, #0b0010
        msr spsel, #0
     	b	_exception_exit
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        bl _exception_entry
	    bl	c_irq_handler
     	b	_exception_exit
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
    .balign 128
        b hang
