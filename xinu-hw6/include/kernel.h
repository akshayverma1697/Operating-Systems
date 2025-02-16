/**
 * @file kernel.h 
 *
 * The base include file for the Xinu kernel. Defines symbolic constants,
 * universal return constants, intialization constants, machine size
 * definitions, inline utility functions, and include types
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stddef.h>
#include <stdarg.h>

/* Kernel function prototypes */
void nulluser(void);

/* Kernel function prototypes */
syscall kprintf(const char *fmt, ...);
syscall kputc(uchar);
syscall kungetc(uchar);
syscall kgetc(void);
syscall kcheckc(void);

/* Scheduler function prototypes */
syscall create(void *funcaddr, ulong ssize, ulong priority, char *name, ulong nargs, ...);
syscall kill(pid_typ pid);
syscall ready(pid_typ pid, bool resched, uint core);
syscall resched(void);

/* Kernel constants */
#define PREEMPT		FALSE		/* preemptive rescheduling			*/
#define AGING		TRUE 	/* queue aging prevents starvation	*/

/* Assertions */
#define ASSERT(cond)              if ( !(cond) ) return SYSERR
#define ASSERTFUNC(func_call)     ASSERT((func_call) == OK)

#endif                          /* _KERNEL_H_ */
