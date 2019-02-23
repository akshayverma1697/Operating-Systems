/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

void testcases(void);

/**
 * Main thread.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.  The default is designed to do something reasonable
 * on all platforms based on the devices and features configured.
 */
process main(void)
{
	int i = 0;
	uint cpuid = getcpuid();

	kprintf("Hello Xinu World! Core %d\r\n", cpuid);

	for (i = 0; i < 10; i++)
	{
		kprintf("This is process %d\r\n", currpid[cpuid]);

		/* Uncomment the resched() line below to see cooperative scheduling. */
		// resched();
	}

	return 0;
}
