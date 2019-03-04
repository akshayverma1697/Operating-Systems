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
	irqmask	im;
	uint cpuid = getcpuid();

    ASSERT(!isbadpid(pid));

	im = disable();

    ppcb = &proctab[pid];
    ppcb->state = PRREADY;
	
	if (-1 == ppcb->core_affinity)
	{
		ppcb->core_affinity = core;
	}

	// TODO: enqueue the process onto the proper readylist.
	//
	//       You will do this based off the processes core_affinity and priority.
	//       
	// NOTE:
	//       readylist is a 2-Dimensional array..
	//       the first index indicates what core, 
	//       and the second index indicates which priority it is.
	//       ex. readylist[0][2] will access the readylist for core 0 of the highest priority (2).
	//
	// NOTE2:
	//       The core_affinity and priority are values you should 
	//       be able to get from the process's process control block.




	/* resched if flag is set and if the */
	/* processes affinity is the same as the */
	/* current processor (cpuid)...      */
    if (resch && (cpuid == ppcb->core_affinity))
    {
        resched();
    }
	restore(im);
    return OK;
}
