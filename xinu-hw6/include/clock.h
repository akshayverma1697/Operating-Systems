/**
 * @file clock.h
 * Definitions relating to the hardware clock and the Xinu clock interrupt.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stddef.h>

/**
 * @ingroup timer
 *
 * Timer resolution.  This refers to the maximum number of times that Xinu
 * schedules the timer interrupt per second, @a not to the number of native
 * clock cycles per second.
 */
#define CLKTICKS_PER_SEC  1000


#define QUANTUM 3 	/* clock ticks until preemption	*/

/**
 * These are your "quanta's". 
 *
 * The "preempt" quantum indicates when the scheduler should preempt.
 * For example, if the QUANTUM is 3, then after 3 clock ticks, the scheduler will preempt
 * and then reset the value of the preempt quantum to QUANTUM.
 *
 * The "promote_medium" quantum indicates when a medium priority process ("PRIORITY_MED")
 * should get promoted and become a high priority process ("PRIORITY_HIGH").
 * The promote_medium quantum should decrement when resched() is called and AGING is TRUE.
 *
 * The "promote_low" quantum indicates when a low priority process ("PRIORITY_LOW")
 * should get promoted and become a medium prority process. This quantum should be decremented
 * every time a medium priority process is promoted.
 *
 * NOTE: The quantums are array's because there is one for each core.
 *       When accessing these, the proper way to do so would be to 
 *       use the cpuid as an index.
 *
 * NOTE2: The "promote_medium[]" and "promote_low[]" quantum's should only be used
 *        when AGING is set to TRUE.
 *
 * NOTE3: All of theses quantum's have a reset value of QUANTUM (#defined above this).
 *        Therefore, when these reach 0, they should be reset to that value.
 */
extern volatile ulong preempt[];	
extern volatile ulong promote_medium[];
extern volatile ulong promote_low[];

/**
 * clkticks[] and clktime[] keep track of information about the clock.
 * These are arrays because there is one for each core (each core has its own clock).
 */
extern volatile ulong clkticks[];
extern volatile ulong clktime[];

/* Clock function prototypes.  Note:  clkupdate() and clkcount() are documented
 * here because their implementations are platform-dependent.  */

void clkinit(void);

/**
 * @ingroup timer
 *
 * Sets up a timer interrupt to trigger after a certain number of clock cycles
 * have elapsed.
 *
 * @param cycles
 *     Number of cycles after which the timer interrupt is to be triggered.
 *     This refers to native clock cycles (whose frequency is specified in
 *     platform::clkfreq).
 */
void clkupdate(ulong cycles);

/**
 * @ingroup timer
 *
 * Gets the current system timer cycles.
 *
 * @return
 *	The current number of timer cycles.  Generally, only the difference between
 *	the value returned by between two successive calls to this function is
 *	meaningful.  The number of cycles that elapse per second is specified by the
 *	@ref platform::clkfreq "value in platform".
 */
ulong clkcount(void);

interrupt clkhandler(void);
void udelay(ulong);
void mdelay(ulong);

#endif                          /* _CLOCK_H_ */
