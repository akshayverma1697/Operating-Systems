//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
//
/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 *
 * and
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

#define total_bytes 16 //4 slots with 4 bytes (4*4=16)//

extern void main(int, char *);


/* student created function to create process recursively */
void akshaySpecial(int n)
{
    if(n < 15) // number to limit recursion - limited to 11 calls
    {
        n = 1 + create((void *)akshaySpecial, INITSTK, "AKSHAY&JOSH", 2,0,NULL); // create process
        kprintf("This is our added test case boi\n\r"); // print proof process is called again
        akshaySpecial(n); // recursive call with new n
    }
}

int testmain(int argc, char **argv)
{
	uint cpuid = getcpuid();
    int i = 0;
    kprintf("Hello XINU World, from core %d!\r\n", cpuid);

    for (i = 0; i < 10; i++)
    {
        kprintf("This is process %d\r\n", currpid[cpuid]);

        /* Uncomment the resched() line for cooperative scheduling. */
       	resched();
    }
    return 0;
}

void testbigargs(int a, int b, int c, int d, int e, int f, int g, int h)
{
    kprintf("Testing bigargs...\r\n");
    kprintf("a = 0x%08X\r\n", a);
    kprintf("b = 0x%08X\r\n", b);
    kprintf("c = 0x%08X\r\n", c);
    kprintf("d = 0x%08X\r\n", d);
    kprintf("e = 0x%08X\r\n", e);
    kprintf("f = 0x%08X\r\n", f);
    kprintf("g = 0x%08X\r\n", g);
    kprintf("h = 0x%08X\r\n", h);
}

void printpcb(int pid)
{
    pcb *ppcb = NULL;

    /* Using the process ID, access it in the PCB table. */
    ppcb = &proctab[pid];

    /* Printing PCB */
    kprintf("Process name		  : %s \r\n", ppcb->name);

    switch (ppcb->state)
    {
    case PRFREE:
        kprintf("State of the process	  : FREE \r\n");
        break;
    case PRCURR:
        kprintf("State of the process 	  : CURRENT \r\n");
        break;
    case PRSUSP:
        kprintf("State of the process	  : SUSPENDED \r\n");
        break;
    case PRREADY:
        kprintf("State of the process	  : READY \r\n");
        break;
    default:
        kprintf("ERROR: Process state not correctly set!\r\n");
        break;
    }

    /* Print PCB contents and registers */
    kprintf("Base of run time stack    : 0x%08X \r\n", ppcb->stkbase);
    kprintf("Stack length of process   : %8u \r\n", ppcb->stklen);
    kprintf("Stack pointer: 0x%08X \r\n", ppcb->regs[PREG_SP]); // print Stack pointer
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c, pid;
    int *stpr; //create stack pointer
    pcb *ppcb; // initializes pointer
    kprintf("0) Test creation of one process\r\n");
    kprintf("1) Test passing of many args\r\n");
    kprintf("2) Create three processes and run them\r\n");
	kprintf("3) Create three processes and run them on other cores\r\n");
    kprintf("4) Create a process that will create more of itself\r\n");

    kprintf("===TEST BEGIN===\r\n");

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
		// Process creation testcase
        pid = create((void *)testmain, INITSTK, "MAIN1", 2, 0, NULL);
        printpcb(pid);
        break;

    case '1':
		// Many arguments testcase
        pid = create((void *)testbigargs, INITSTK, "MAIN1", 8,
                     0x11111111, 0x22222222, 0x33333333, 0x44444444,
                     0x55555555, 0x66666666, 0x77777777, 0x88888888);
        printpcb(pid);
        // TODO: print out stack with extra args
        
        
        ppcb = &proctab[pid]; // accesses current process
        stpr = ppcb->regs[PREG_SP]; // get the stack pointer
        
        for(; stpr < *stpr + total_bytes; stpr++ )
        {
            kprintf("%X \r\n", *stpr);
        }
        
        
        
        // TODO: ready(pid, RESCHED_YES, 0);
		ready(pid, RESCHED_YES, 0);
        break;

    case '2':
        // Create three copies of a process, and let them play.
        ready(create((void *)testmain, INITSTK, "MAIN1", 2, 0, NULL), RESCHED_NO , 0);
        ready(create((void *)testmain, INITSTK, "MAIN2", 2, 0, NULL), RESCHED_NO , 0);
        ready(create((void *)testmain, INITSTK, "MAIN3", 2, 0, NULL), RESCHED_YES, 0);
        break;
	
	case '3':
		// Create 3 processes and ready them on cores 1, 2, 3
		ready(create((void *)testmain, INITSTK, "MAIN1", 2, 0, NULL), RESCHED_NO, 1);
		ready(create((void *)testmain, INITSTK, "MAIN2", 2, 0, NULL), RESCHED_NO, 2);
		ready(create((void *)testmain, INITSTK, "MAIN3", 2, 0, NULL), RESCHED_NO, 3);
		break;
        
    case '4':
        // Create a process that creates itself 
        akshaySpecial(0);
        //pid = create((void *)akshaySpecial, INITSTK, "MAIN1", 2, 0, NULL);
        //printpcb(pid);
	default:
		break;
    }

    kprintf("\r\n===TEST END===\r\n");
    return;
}
