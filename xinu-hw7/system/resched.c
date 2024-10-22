//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
	int highest_prio;
    //pid_typ pid;
	irqmask im;
	pcb *oldproc;               /* pointer to old process entry */
	pcb *newproc;               /* pointer to new process entry */

	uint cpuid = getcpuid();

	oldproc = &proctab[currpid[cpuid]];

	im = disable();

#if AGING

	// TODO: Implement aging.
	//
	//       Reference include/clock.h to find more information
	//       about the quantums and how aging should behave.
    
    promote_medium[cpuid]--; //decrement promote_medium cpuid. PUT ABOVE "IF" because you must first decrement the timer before you are able to enter the first if statement
    
    if(promote_medium[cpuid] <= 0)//enter if statement once the medium timer reaches 0, 
        //READ: the "<" sign is a saftey net in case the timer ever goes under 0, and the computer never saw the timer hit 0//
    {
        if(nonempty(readylist[cpuid][PRIORITY_MED]))// if a process is available, enter if statement in order to move the process to high priority
        {
            enqueue((dequeue(readylist[cpuid][PRIORITY_MED])),(readylist[cpuid][PRIORITY_HIGH]));// move process to high priority, 
            //READ: dequeue returns a PID which is why it is used as the first argument in enqueue for both priorities (medium and low)//
        }
        promote_medium[cpuid] = QUANTUM;// reset medium quantum
        
        promote_low[cpuid]--;//decrement promote_low cpuid  
            
        if(promote_low[cpuid] <= 0)//enter if statement when timer reaches 0
        {
            if(nonempty(readylist[cpuid][PRIORITY_LOW])) //enter if there is a process in the low queue
            {
            enqueue((dequeue(readylist[cpuid][PRIORITY_LOW])) , (readylist[cpuid][PRIORITY_MED]));// promote to medium queue  
            }
        promote_low[cpuid] = QUANTUM;// reset low quantum timer        
        }        
        
    }
    
    
    

#endif

	/* place current process at end of ready queue */
	if (PRCURR == oldproc->state)
	{
		oldproc->state = PRREADY;
		enqueue(currpid[cpuid], readylist[cpuid][oldproc->priority]);
	}

	/* remove first process in highest priority ready queue */
	// determine queue to pick from
	if (nonempty(readylist[cpuid][PRIORITY_HIGH]))
	{
		highest_prio = PRIORITY_HIGH;
	}
	else if (nonempty(readylist[cpuid][PRIORITY_MED]))
	{
		highest_prio = PRIORITY_MED;
	}
	else
	{
		highest_prio = PRIORITY_LOW;
	}

	currpid[cpuid] = dequeue(readylist[cpuid][highest_prio]);
	newproc = &proctab[currpid[cpuid]];
	newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
	preempt[cpuid] = QUANTUM;
#endif

	ctxsw(&oldproc->regs, &newproc->regs);

	/* The OLD process returns here when resumed. */
	restore(im);
	return OK;
}
