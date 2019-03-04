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
	
	enable();

	kprintf("Hello Xinu World! Core %d\r\n", cpuid);

	for (i = 0; i < 5; i++)
	{
		kprintf("This is process %d\r\n", currpid[cpuid]);
		
		/* have to include a slight delay to see the effects of preemption.. */
		udelay(1);
	}

	return 0;
}
