/**
 * @file initialize.c
 * The system begins intializing after the C environment has been
 * established.  After intialization, the null thread remains always in
 * a ready (THRREADY) or running (THRCURR) state.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
static int sysinit(void);       /* intializes system structures        */
static void print_os_info(void);	/* Print inital O/S data	*/
extern void testcases(void);	 /* Testing hook */
extern process main(void);       /* Main program    */
static void core_nulluser(void); /* Null process for secondary cores   */

/* Declarations of major kernel variables */
pcb proctab[NPROC];				/* Process table					   */
qid_typ readylist[NCORES];		/* List of READY processes			   */

/* Active system status */
int numproc;					/* Number of live user processes       */
int currpid[NCORES];			/* Id of currently running proccesses  */

/* Params set by startup.S */
void *memheap;                  /* Bottom of heap (top of O/S stack)   */
ulong cpuid;                    /* Processor id                        */

struct platform platform;       /* Platform specific configuration     */

spinlock_t serial_lock;

/*
 * Intializes the system and becomes the null thread.
 * This is where the system begins after the C environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  This routine turns itself into the null thread
 * after initialization.  Because the null thread must always remain ready
 * to run, it cannot execute code that might cause it to be suspended, wait
 * for a semaphore, or put to sleep, or exit.  In particular, it must not
 * do I/O unless it uses kprintf for synchronous output.
 */
void nulluser(void)
{
	uint cpuid = getcpuid();

	/* Platform-specific initialization */		
	platforminit();

	/* General initialization  */
	sysinit();

	/* Unpark the other cores */
	unparkcore(1, (void *) core_nulluser, NULL);
	unparkcore(2, (void *) core_nulluser, NULL);
	unparkcore(3, (void *) core_nulluser, NULL);

	/* Standard Embedded Xinu processor and memory info */
	print_os_info();

	/* Call the testcases */
	testcases();

	/* Call the main program */
//	ready(create((void *) main, INITSTK, "MAIN", 2, 0, NULL), 0, 0);

	/* null thread has nothing else to do but cannot exit  */
	while (1)
	{
		if (nonempty(readylist[cpuid]))
		{
			resched();
		}	
	}

}

/**
 * Intializes all Xinu data structures and devices.
 * @return OK if everything is initialized successfully
 */
static int sysinit(void)
{
	int i = 0;
	pcb *ppcb = NULL;           /* process control block pointer */

	/* Initialize serial lock */
	serial_lock = lock_create();

	/* Initialize system variables */
	numproc = 4;               /* 4 initial null processes running */

	/* Initialize process table */
	for (i = 0; i < NPROC; i++)
	{
		proctab[i].state = PRFREE;
		proctab[i].core_affinity = -1;
	}

	/* Initialize null process entries */
	ppcb = &proctab[NULLPROC];
	ppcb->state = PRCURR;
	strncpy(ppcb->name, "prnull", PNMLEN);
	ppcb->stkbase = (void *) &_end;
	ppcb->regs[PREG_SP] = NULL;
	ppcb->stklen  = 8192;
	ppcb->core_affinity = 0;
	currpid[0] = NULLPROC;

	ppcb = &proctab[NULLPROC1];
	ppcb->state = PRCURR;
	strncpy(ppcb->name, "prnull01", PNMLEN);
	ppcb->stkbase = (void *)(&_end + 8192);
	ppcb->regs[PREG_SP] = NULL;
	ppcb->stklen  = 8192;
	ppcb->core_affinity = 1;
	currpid[1] = NULLPROC1;

	ppcb = &proctab[NULLPROC2];
	ppcb->state = PRCURR;
	strncpy(ppcb->name, "prnull02", PNMLEN);
	ppcb->stkbase = (void *)(&_end + 16384);
	ppcb->regs[PREG_SP] = NULL;
	ppcb->stklen  = 8192;
	ppcb->core_affinity = 2;
	currpid[2] = NULLPROC2;

	ppcb = &proctab[NULLPROC3];
	ppcb->state = PRCURR;
	strncpy(ppcb->name, "prnull03", PNMLEN);
	ppcb->stkbase = (void *)(&_end + 24576);
	ppcb->regs[PREG_SP] = NULL;
	ppcb->stklen  = 8192;
	ppcb->core_affinity = 3;
	currpid[3] = NULLPROC3;

	/* Initialize ready lists */
	for (i = 0; i < NCORES; i++)
		readylist[i] = newqueue();

	return OK;
}

static void core_nulluser(void)
{
	extern void pld(void *);
	uint cpuid = getcpuid();

	while (1)
	{
		pld(&queuetab[readylist[cpuid]].next);
		if (nonempty(readylist[cpuid]))
		{
			resched();
		}
	}
}

static void print_os_info(void)
{
	kprintf(VERSION);
	kprintf("\r\n\r\n");

	/* Output detected platform. */
	kprintf("Detected platform as: %s, %s\r\n\r\n",
			platform.family, platform.name);

	/* Output Xinu memory layout */
	kprintf("%10d bytes physical memory.\r\n",
			(ulong)platform.maxaddr - (ulong)platform.minaddr);
	kprintf("           [0x%08X to 0x%08X]\r\n",
			(ulong)platform.minaddr, (ulong)(platform.maxaddr - 1));

	extern void _start(void);
	kprintf("%10d bytes reserved system area.\r\n",
			(ulong)_start - (ulong)platform.minaddr);
	kprintf("           [0x%08X to 0x%08X]\r\n",
			(ulong)platform.minaddr, (ulong)_start - 1);

	kprintf("%10d bytes Xinu code.\r\n", (ulong)&_end - (ulong)_start);
	kprintf("           [0x%08X to 0x%08X]\r\n",
			(ulong)_start, (ulong)&_end - 1);

	kprintf("%10d bytes stack space.\r\n", (ulong)memheap - (ulong)&_end);
	kprintf("           [0x%08X to 0x%08X]\r\n",
			(ulong)&_end, (ulong)memheap - 1);

	kprintf("%10d bytes heap space.\r\n",
			(ulong)platform.maxaddr - (ulong)memheap);
	kprintf("           [0x%08X to 0x%08X]\r\n\r\n",
			(ulong)memheap, (ulong)platform.maxaddr - 1);
	kprintf("\r\n");
}
