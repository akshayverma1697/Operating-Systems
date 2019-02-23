/**
 * @file ready.c
 * @provides ready
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

/**
 * Make a process eligible for CPU service.
 * @param pid process id of the process to move to the ready state
 * @param resch if TRUE, reschedule will be called
 * @return OK if the process has been added to the ready list, else SYSERR
 */
syscall ready(pid_typ pid, bool resch, uint core)
{
    register pcb *ppcb;
	uint cpuid = getcpuid();

    ASSERT(!isbadpid(pid));

    ppcb = &proctab[pid];
    ppcb->state = PRREADY;
	
	if (-1 == ppcb->core_affinity)
	{
		ppcb->core_affinity = core;
	}

    enqueue(pid, readylist[ppcb->core_affinity]);

	/* resched if flag is set and if the */
	/* processes affinity is the same as the */
	/* current processor (cpuid)...      */
    if (resch && (cpuid == ppcb->core_affinity))
    {
        resched();
    }
    return OK;
}
