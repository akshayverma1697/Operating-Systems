/**
 * @file     clkinit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/** @ingroup timer
 *
 * Number of timer interrupts that have occurred since ::clktime was
 * incremented.  When ::clkticks reaches ::CLKTICKS_PER_SEC, ::clktime is
 * incremented again and ::clkticks is reset to 0.
 */
volatile ulong clkticks[NCORES];

/** @ingroup timer
 * Number of seconds that have elapsed since the system booted.  */
volatile ulong clktime[NCORES];

#if PREEMPT
volatile ulong preempt[NCORES];
#endif

#if AGING
volatile ulong promote_medium[NCORES];
volatile ulong promote_low[NCORES];
#endif

/**
 * @ingroup timer
 *
 * Initialize the clock and sleep queue.  This function is called at startup.
 */
void clkinit(void)
{
	int i;
#if PREEMPT
	for (i = 0; i < NCORES; i++)
	{
		preempt[i]        = QUANTUM;
	}
#endif

#if AGING
	for (i = 0; i < NCORES; i++)
	{
		promote_medium[i] = QUANTUM;
		promote_low[i]    = QUANTUM;
	}
#endif

	for (i = 0; i < NCORES; i++)
	{
    	clkticks[i] = 0;
		clktime[i]  = 0;
	}

    kprintf("Time base %dHz, Clock ticks at %dHz\r\n",
            platform.clkfreq, CLKTICKS_PER_SEC);

    /* register clock interrupt */
    interruptVector[IRQ_TIMER] = clkhandler;
    enable_irq(IRQ_TIMER);
    clkupdate(platform.clkfreq / CLKTICKS_PER_SEC);

}
