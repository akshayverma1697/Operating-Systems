/**
 * @file start.S
 *
 * Initialization code for Embedded Xinu on the Raspberry Pi.
 *
 * Embedded Xinu, Copyright (C) 2013.  All rights reserved.
 *
 * -----------------------------------------------------------------------------------------
 * 
 * Specific for Raspberry Pi 3 (BCM2837) - Memory Layout (Not to scale)
 * 1GB OF RAM (PHYSICAL MEMORY) AVAILABLE. 1GB == 0x3FFFFFFF
 * +----------+
 * |	      |
 * | IO SPACE | -> 0x3F000000 to 0x3FFFFFFF reserved for IO (GPIO, UART, SYS TIMER, USB CORE)
 * |          |
 * +----------+
 * |          |
 * |          |
 * |   HEAP   | -> getmem allocates from here
 * |          |
 * |          |
 * +----------+
 * | OS STACK | -> becomes the null process' stack
 * +----------+
 * |   BSS    | -> needed for C environment
 * +----------+
 * |   TEXT   | -> XINU code
 * |          |
 * +----------+
 * | RESERVED | -> interrupt handler and vectors go here
 * +----------+
 */

#include <arm.h>     /* For ARM_MODE_SYS */

#define NULLSTK 8192

.section .init
	.globl _start

	/* _start:  Entry point of the Xinu kernel.  This will be the very first
	/* byte of the kernel image and on the Raspberry Pi 3 (32-bit mode) will be loaded by
	/* the GPU at address 0x8000.  */

	.func _start
_start:
	/* Save the pointer to the atags (ARM boot tags). The Pi 3 *does* pass atags,
	 * and does so into address 0x100. We access it there, just like the Pi 1.
	 * The standard protocol for Aarch32 (ARMv8-A) is to pass the atags pointer
	 * into register r2. We then store it in r3, to be accessed by C code which
	 * parses the atags. */
	ldr 	r3, =atags_ptr
	cmp	r2, #0
	moveq   r2, #0x100
	str 	r2, [r3]

	/* Check if processor is already in SYSTEM mode,
	 * if it is, jump to reset handler. */
	mrs	r0, cpsr
	and	r0, r0, #0b11111
	cmp	r0, #ARM_MODE_SYS
	beq	reset_handler
	

       /* Upon boot, the device loads into HYP (hypervisor) mode, which does not have permission to
	* access the CPS opcode. Therefore, to enter the most privelaged mode,
	* SYSTEM mode (11111), we first need to enter SUPERVISOR mode. This allows more privelage,
	* so that the "cps" opcode may be used to switch to SYSTEM mode.
	* First clear bits of CPSR, then enter supervisor mode. Increment the program counter
        * by four to skip a 32-bit word. Upon exception return, the CPSR is changed, and
	* we are in SUPERVISOR mode. This continues at reset handler, where SYSTEM mode is set. */
	
	
	/* Go from hypervisor mode to system mode */
	/* Required to use exception return method */
	/* Continues execution at line "b reset_handler" */
	mrs	r0, cpsr
//	bic	r0, r0, #ARM_MODE_SYS
	orr	r0, r0, #ARM_MODE_SYS
	msr	spsr_cxsf, r0
	add	r0, pc, #4
	msr	ELR_hyp, r0
	eret
        
	/* Continue at reset handler. */
	b reset_handler
	.endfunc

/* ARM exception vector table.  This is copied to VBAR register. */
/* Vector table has to be 32-byte aligned. */
.balign	0x20
.globl _vectors
_vectors:
	ldr pc, reset_addr    /* Reset handler */
	ldr pc, undef_addr	  /* Undefined instruction handler */
	ldr pc, swi_addr	  /* Software interrupt handler */
	ldr pc, prefetch_addr /* Prefetch abort handler */
	ldr pc, abort_addr	  /* Data abort handler */
	ldr pc, reserved_addr /* Reserved */
	ldr pc, irq_addr	  /* IRQ (Interrupt request) handler */
	ldr pc, fiq_addr	  /* FIQ (Fast interrupt request) handler */

reset_addr:     .word reset_handler
undef_addr:     .word reset_handler
swi_addr:       .word reset_handler
prefetch_addr:  .word reset_handler
abort_addr:     .word reset_handler
reserved_addr:  .word reset_handler
irq_addr:       .word reset_handler
fiq_addr:       .word reset_handler

_endvectors:

.section .text

	/* reset_handler: Reset handler routine executed to start up the kernel,
	 * when the ARM processor is reset, or (currently) when an unhandled
	 * exception occurs.  */
	.func reset_handler
reset_handler:

	/* After entering SUPERVISOR mode, enter SYSTEM mode with all interrupts
	 * disabled so the ARM processor is in a known state. */
//	cpsid	if, #ARM_MODE_SYS
	cpsid	if
	/* Copy the ARM exception table from .init section to address 0,
	 * including the absolute address table.  Here we assume it is exactly
	 * 16 words.  */
	mov	r0, #0
	ldr	r1, =_vectors
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

	mov	r1, #0
	mcr	p15, 0, r1, c12, c0, 0	/* Write to VBAR */
					/* Vectors are at address 0x0 */

	/* Enable the following features by modifying the ARM processor's
	 * Control Register:
	 *
	 * - unaligned memory accesses   (bit 22)
	 *
	 * Note: Xinu shouldn't do any unaligned memory accesses on purpose, but
	 * we found that gcc can still generate unaligned memory accesses for
	 * tasks such as copying character strings.  (An alternative would be to
	 * pass -mno-unaligned-access to gcc to prevent it from doing so.)  */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, #1 << 22
	mcr p15, 0, r0, c1, c0, 0

	/* Clear the .bss section of the kernel.  */
	ldr r0, =_bss
	ldr r1, =_end
  	mov r2, #0
    mov r3, #0
	mov r4, #0
	mov r5, #0
  	b bssloopa

bssloopb:
	stmia r0!, {r2-r5}

bssloopa:
	cmp r0, r1 	/* check that we still haven't hit the end of bss yet */
	blo bssloopb	/* if still below, go backwards and loop */

	ldr	r3, =core_init_sp

	/* Put the null thread's stack directly after the kernel image.  */
	add sp, r1, #NULLSTK	/* Core 0 stack pointer */
	str	sp, [r3, #0]	
	
	add	r0, sp, #NULLSTK	/* Core 1 stack pointer */
	str	r0, [r3, #4]

	add	r1, r0, #NULLSTK	/* Core 2 stack pointer */
	str	r1, [r3, #8]

	add	r2, r1, #NULLSTK	/* Core 3 stack pointer */
	str	r2, [r3, #12]


	/* The remaining memory available to the ARM will be Xinu's "memheap"
	 * region, which is used for dynamic memory allocation.  Set its
	 * starting address.  */
	ldr r0, =memheap
	str r2, [r0]		/* use r2 because this is the last stack allocated for core 3 */


	/* Branch to nulluser function, located in xinu/system/initialize.c
	/* The function will initialize the system and become the null thread. */
	b	nulluser
	.endfunc


.globl dmb
dmb:
	.func dmb
	dmb
	mov	pc, lr
	.endfunc

.globl pld
pld:
	.func pld
	pld	[r0]
	bx	lr
	.endfunc

.globl pldw
pldw:
	.func pldw
	pldw [r0]
	bx	lr
	.endfunc

.globl _atomic_increment_post
/**
 * @fn int _atomic_increment_post(int *var)
 *
 * Atomically increments specified integer.
 * (Equivalent to var++ in C).
 *
 * @param *var pointer to variable
 * @return value the variable held prior
 *         to being incremented.
 */
_atomic_increment_post:
	.func _atomic_increment_post
	pldw	[r0]
	ldrex	r1, [r0]	// load
	mov		r3, r1		// store value in r3
	add		r1, #1		// increment
	strex	r2, r1, [r0] // store
	cmp		r2, #0		// if the store fails, try again
	bne		_atomic_increment_post

	mov		r0, r3		// return value from r3
	dmb

	bx	lr
	.endfunc

.globl _atomic_increment_pre
/**
 * @fn int _atomic_increment_pre(int *var)
 *
 * Atomically increments specified integer.
 * (Equivalent to ++var in C).
 *
 * @param *var pointer to variable
 * @return value the variable held after
 *         being incremented.
 */
_atomic_increment_pre:
	.func _atomic_increment_pre
	pldw	[r0]
	ldrex	r1, [r0]	// load
	add		r1, #1		// increment
	strex	r2, r1, [r0] // store
	cmp		r2, #0		// if store fails, try again
	bne		_atomic_increment_pre
	
	mov	r0, r1			// return incremented value
	dmb

	bx	lr
	.endfunc

.globl _atomic_increment_limit
/**
 * @fn int _atomic_increment_limit(int *var, int limit)
 *
 * Atomically increment and wrap-around the limit.
 * Equivalent to var = (var + 1) % limit.
 * 
 * @param *var  pointer to variable
 * @param limit value to wrap-around if
 *              var exceeds it.
 * @return incremented var value
 */
_atomic_increment_limit:
	.func _atomic_increment_limit
	pldw	[r0]
	ldrex	r2, [r0] 		// load
	add		r2, #1			// increment
	cmp		r2, r1			// if >= to limit, wrap to zero
	movge	r2, #0
	strex	r3, r2, [r0]	// store
	cmp		r3, #0			// if it fails, retry
	bne		_atomic_increment_limit

	mov		r0, r2
	dmb

	bx	lr
	.endfunc	


.globl _atomic_decrement_post
/**
 * @fn int _atomic_decrement_post(int *var)
 *
 * Atomically decrements specified integer.
 * (Equivalent to var-- in C).
 *
 * @param *var pointer to variable
 * @return value the variable held prior
 *         to being deccremented.
 */
_atomic_decrement_post:
	.func _atomic_decrement_post
	pldw	[r0]
	ldrex	r1, [r0]	// load
	mov		r3, r1		// store value in r3
	sub		r1, #1		// decrement
	strex	r2, r1, [r0] // store
	cmp		r2, #0		// if the store fails, try again
	bne		_atomic_decrement_post

	mov		r0, r3		// return value from r3
	dmb

	bx	lr
	.endfunc

.globl _atomic_decrement_pre
/**
 * @fn int _atomic_decrement_pre(int *var)
 *
 * Atomically decrements specified integer.
 * (Equivalent to --var in C).
 *
 * @param *var pointer to variable
 * @return value the variable held after
 *         being decremented.
 */
_atomic_decrement_pre:
	.func _atomic_decrement_pre
	pldw	[r0]
	ldrex	r1, [r0]	// load
	sub		r1, #1		// decrement
	strex	r2, r1, [r0] // store
	cmp		r2, #0		// if store fails, try again
	bne		_atomic_decrement_pre
	
	mov	r0, r1			// return decremented value
	dmb

	bx	lr
	.endfunc
