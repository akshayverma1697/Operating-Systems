//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

void printpid(int times)
{
	int i = 0;
	uint cpuid = getcpuid();

	enable();
	for (i = 0; i < times; i++)
	{
		kprintf("This is process %d\r\n", currpid[cpuid]);
        //kprintf("promote_medium timer: %d\r\n", promote_medium[cpuid]);//prints medium timer
        //kprintf("promote_low timer: %d\r\n", promote_low[cpuid]);//prints low timer
        resched();
		//udelay(1);
	}
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;

	kprintf("===TEST BEGIN===\r\n");
	kprintf("0) Test priority scheduling\r\n");
	kprintf("\'A\') Aging / Starvation testcase\r\n");
	kprintf("\'P\') Preemption testcase\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			// Run 3 processes with varying priorities
			ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1, 5), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_MED , "PRINTER-B", 1, 5), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_LOW , "PRINTER-C", 1, 5), RESCHED_YES, 0);

			break;

		case 'a':
		case 'A':
#if AGING
			// AGING TESTCASE medium will be promoted to high and low to medium so they have a chance to run before high priority finishes -- prevents starvation
            // process with lower number runs first but higher number processes will begin printing before lower number finishes
			kprintf("AGING is enabled.\r\n");
			
			// TODO: Create a testcase that demonstrates aging 
            
            ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-HIGH", 1, 5), RESCHED_NO, 0);
            ready(create((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-MED", 1, 5), RESCHED_NO, 0);            
            ready(create((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-LOW", 1, 5), RESCHED_YES, 0);

#else
			// STARVING TESTCASE processes will run in order according to priority
            // process numbers run in incrementing order with no interrupts from latter processes
			kprintf("\r\nAGING is not currently enabled.\r\n");
			
			// TODO: Create a testcase that demonstrates starvation
            ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-HIGH", 1, 5), RESCHED_NO, 0);
            ready(create((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-MED", 1, 5), RESCHED_NO, 0);            
            ready(create((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-LOW", 1, 5), RESCHED_YES, 0
#endif
			break;

		case 'p':
		case 'P':
#if PREEMPT
			// PREEMPTION TESTCASE low priority executed first, but is then preempted by high priority
            // process with higher number will run before process with lower number
			kprintf("\r\nPreemption is enabled.\r\n");

			// TODO: Create a testcase that demonstrates preemption
            ready(create((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-MED", 1, 10), RESCHED_NO, 0);
            ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-HIGH", 1, 10), RESCHED_YES, 0);

#else
			// process will not be preempted, and the lower priority process will run fully before the higher priority process
			kprintf("\r\nPreemption is not currently enabled...\r\n");
            ready(create((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-MED", 1, 10), RESCHED_NO, 0);
            ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-HIGH", 1, 10), RESCHED_YES, 0);
#endif
			break;
		
		default:
			kprintf("No aging or preemption chosen.\r\n");
			break;
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
