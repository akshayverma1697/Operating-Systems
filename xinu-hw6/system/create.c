//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
//
/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <arm.h>
#include <xinu.h>

/* Assembly routine for atomic operations */
extern int _atomic_increment_post(int *);
extern int _atomic_increment_limit(int *, int);

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, ulong priority, char *name, ulong nargs, ...) // added priority field to be after name
{
	ulong *saddr;               /* stack address                */
	ulong pid;                  /* stores new process id        */
	pcb *ppcb;                  /* pointer to proc control blk  */
	ulong i;
	va_list ap;                 /* points to list of var args   */
	ulong pads = 0;             /* padding entries in record.   */
	void INITRET(void);

	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
	/* round up to even boundary    */
	saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
	pid = newpid();
	/* a little error checking      */
	if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
	{
		return SYSERR;
	}

	_atomic_increment_post(&numproc);

	ppcb = &proctab[pid];
	/* setup PCB entry for new proc */
	ppcb->state = PRSUSP;
    
	// TODO: Setup PCB entry for new process.
    ppcb->stklen = ssize; // setting stack length
    ppcb->priority = priority; //setting priority
    ppcb->stkbase = saddr; // setting stack address to stack base
    ppcb->core_affinity = -1;// from notes, set to -1 bcuz we don't know the core yet
    strncpy(ppcb->name, name, PNMLEN); // (dest, src, size) copying parameter *name into struct name
 
    
    
	/* Initialize stack with accounting block. */
	*saddr = STACKMAGIC;
	*--saddr = pid;
	*--saddr = ppcb->stklen;
	*--saddr = (ulong)ppcb->stkbase;

	/* Handle variable number of arguments passed to starting function   */
	if (nargs)
	{
		pads = ((nargs - 1) / 4) * 4;
	}
	/* If more than 4 args, pad record size to multiple of native memory */
	/*  transfer size.  Reserve space for extra args                     */
	for (i = 0; i < pads; i++)
	{
		*--saddr = 0;
	}

	// TODO: Initialize process context.
	
	// TODO:  Place arguments into activation record.
	//        See K&R 7.3 for example using va_start, va_arg and
	//        va_end macros for variable argument functions.    
    
    // set up link register and program counter and stack pointer
	ppcb->regs[PREG_SP] = (int) saddr;
    ppcb->regs[PREG_LR] = (int) userret;
    ppcb->regs[PREG_PC] = (int) funcaddr;
    
    va_start(ap, nargs);
	for(i = 0; i < nargs; i++) // saves registers 4-11 into pcb
    {
        if(i < 4) //first four registers (0,1,2,3)
        {
            ppcb->regs[i] = va_arg(ap, int);// saves variable argument into registers
        }
		else // callee save registers (4-11)
		{
            *(saddr + i - 4) = va_arg(ap, int);
            // if there are more arguments than registers, store on the stack
			//increment stack address by 1 (equal to 4 bytes)
		}
    }
    va_end(ap);


	return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
	pid_typ pid;                /* process id to return     */
	static pid_typ nextpid = 0;

	for (pid = 0; pid < NPROC; pid++)
	{                           /* check all NPROC slots    */
		//        nextpid = (nextpid + 1) % NPROC;
		_atomic_increment_limit(&nextpid, NPROC);
		if (PRFREE == proctab[nextpid].state)
		{
			return nextpid;
		}
	}
	return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
	uint cpuid = getcpuid();
	kill(currpid[cpuid]);
}
