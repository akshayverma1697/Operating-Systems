/**
 * @file wait.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup semaphores
 *
 * Wait on a semaphore.
 *
 * If the semaphore's count is positive, it will be decremented and this
 * function will return immediately.  Otherwise, the currently running thread
 * will be put to sleep until the semaphore is signaled with signal() or
 * signaln(), or freed with semfree().
 *
 * @param sem
 *      Semaphore to wait on.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail if @p
 *      sem did not specify a valid semaphore.
 */
syscall wait(semaphore sem)
{
    register struct sement *semptr;
    register pcb *procptr;
    int c;
    unsigned int cpuid;
    irqmask im;

    cpuid = getcpuid();

    im = disable();
    if (isbadsem(sem))
    {
        restore(im);
        return SYSERR;
    }
    procptr = &proctab[currpid[cpuid]];

    semptr = &semtab[sem];


    /* TODO:
     * Make this decrementation atomically-safe
     * so that multiple cores may safely wait for a semaphore.
     */
	c = --semptr->count;

    if (c < 0)
    {
        procptr->state = PRSUSP;
        procptr->sem = sem;
        enqueue(currpid[cpuid], semptr->queue);

        resched();
    }

    restore(im);
    return OK;
}
