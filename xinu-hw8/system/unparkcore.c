/**
 * @file unparkcore.c
 * @provides unparkcore
 *
 * Embedded Xinu, Copyright (C) 2018. All rights reserved.
 */

#include <core.h>

extern void setupCore(void) __attribute__ ((naked));
typedef void (*fn) (void);
extern void sev(void);

/* array for holding the address of the starting point for each core */
/* this is set in unparkcore() and used in setupCore */
void *corestart[4];

/* array for holding the initial stack pointer for each core */
/* these values are set in start.S and used in setupCore */
unsigned int core_init_sp[4];

/* array for holding the arguements to be passed to the core */
void *init_args[4];

/**
 * unparkcore() - gets a core out of the parked state and
 * sets it to begin execution.
 *
 * @param num	core number; 1, 2, or 3
 *        procaddr	address for the core to start executing
 *        args	arguments to be passed to the core (NULL if no arguments)
 */
void unparkcore(int num, void *procaddr, void *args)
{
    if (num > 0 && num < 4)
    {
        corestart[num] = (void *)procaddr;
        init_args[num] = args;

        /* Set the specified cores mailbox address to setupCore */
        /* this will specify to the core to start execution at setupCore */
        *(volatile fn *)(CORE_MBOX_BASE + CORE_MBOX_OFFSET * num) =
            setupCore;

        sev();                  // send event
        // this takes the core out of its sleeping state and allows it to
        // start running code


    }
}
