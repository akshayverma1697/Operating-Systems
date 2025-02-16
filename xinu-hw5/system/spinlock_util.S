/*TA-BOT:MAILTO akshay.verma@marquette.edu joshuah.solito@marquette.edu*/
/**
 * @file spinlock_util.S
 * @provides _lock_acquire _lock_release _atomic_increment
 *
 * Embedded Xinu, Copyright (C) 2018. All rights reserved.
 */

#include <spinlock.h>

/**
 * @fn void _lock_acquire(unsigned int *lock)
 *
 * Uses atomic operations to set a lock variable.
 *
 * @param lock 	pointer to lock variable
 */
.globl _lock_acquire
_lock_acquire:
	.func	_lock_acquire

try: 
	pldw	[r0]		/* Preload Data with intention to write		*/

	// TODO: Implement a spinlock acquire method by:
	//       First, use ldrex to obtain the value at the lock.
	//       Then, check if it is locked (SPINLOCK_LOCKED).
	//	 	If it is, then retry.
	//	 Second, use strex to try to set the lock to SPINLOCK_LOCKED.
	//	 	If strex fails, try from the top.
	
   
    ldrex r1, [r0] /*optaining the lock value at r0 and storing it to r1*/
    cmp r1, #SPINLOCK_LOCKED /*check if the lock at r1 is locked*/
    beq try /*branch if equal try again means if the lock is locked retry _lock_acquire method*/
     /*if not move on*/
    mov r3, #SPINLOCK_LOCKED /* store SPINLOCK_LOCKED into r3*/
    strex r2, r3, [r0] /* try and set the lock to SPINLOCK_LOCKED*/
    cmp r2, #0 /*check to see if the store takes place thus the value returned will be 0*/
    bne try /*if the status is not equal to 0, strex has failed, restart from the top (_lock_acquire)*/
    /*otherwise success*/
	
	dmb			/* Data Memory Barrier	*/
	bx	lr


	.endfunc

/**
 * @fn void _lock_release(unsigned int *lock)
 *
 * Sets lock variable to unlocked.
 *
 * @param lock	pointer to lock variable
 */
.globl _lock_release
_lock_release:
	.func	_lock_release

	mov	r1, #SPINLOCK_UNLOCKED
	dmb			/* Required before accessing protected resource */
	str	r1, [r0]	/* Unlock lock */
	dmb
	bx	lr

	.endfunc


/**
 * @fn int _atomic_lock_check(unsigned int *)
 *
 * Tries to atomically check if a lock is available.
 *
 * @param address of a lock entries state.
 * @return 0 on success, -1 on failure.
 */
.globl _atomic_lock_check
_atomic_lock_check:
	.func _atomic_lock_checkq

	pldw	[r0]		/* Preload data w/ intent to write	*/
	ldrex	r1, [r0]	/* load exclusive the muxtab state 	*/
	cmp	r1, #SPINLOCK_FREE	/* IF state == SPINLOCK_FREE		*/
	beq	_increment	/* THEN increment to SPINLOCK_USED	*/
				/* OTHERWISE clrex and return 		*/
	clrex
	dmb
	mov	r0, #-1
	bx	lr

_increment:
	add	r1, r1, #1	/* increments to SPINLOCK_USED		*/
	strex	r2, r1, [r0]	/* strex SPINLOCK_USED			*/
	cmp	r2, #0x0	/* IF strex doesnt succeed,		*/
	bne	_atomic_lock_check	/* jump back to the top		*/
				/* OTHERWISE return as success		*/
	dmb
	mov	r0, #0
	bx	lr

	.endfunc
