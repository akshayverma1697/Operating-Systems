/**
 * @file kill.c
 * Provides: kill
 *
 * COSC 3250/ COEN 4820 Assignment 4
 */

/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

/* Assembly atomic operations subroutines */
extern int _atomic_decrement_pre(int *);

/*
 * kill  --  kill a process and remove it from the system
 */
syscall kill(int pid)
{
    pcb *ppcb;                  /* points to process control block for pid */
    uint cpuid = getcpuid();

    if (isbadpid(pid) || (0 == pid) || (1 == pid)
        || (2 == pid) || (3 == pid)
        || (cpuid != proctab[pid].core_affinity)
        || (PRFREE == (ppcb = &proctab[pid])->state))
    {
        return SYSERR;
    }

    ppcb = &proctab[pid];

    _atomic_decrement(&numproc);

    if (numproc <= 4)
    {
        // all processes are done running
//              kprintf("\r\n\r\nAll user process have completed.\r\n\r\n");
//              extern void halt(void);
//              halt();
    }

    switch (ppcb->state)
    {
    case PRCURR:
        ppcb->state = PRFREE;   /* suicide */
        resched();

    case PRREADY:
        remove(pid);

    default:
        ppcb->state = PRFREE;
    }

    return OK;
}
