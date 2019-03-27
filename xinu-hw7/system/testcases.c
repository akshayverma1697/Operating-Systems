/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
void print_sem(semaphore sem);
uchar getc(void);
void putc(uchar);


/* Test process to use for testing semaphores. */
process testSemWait(semaphore sem)
{
    wait(sem);
    printf("process %d after wait()\r\n", currpid[getcpuid()]);
    return 0;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;
    int i = 0;
    semaphore testsem;

    enable();

    printf("===TEST BEGIN===\r\n");
    printf("1)A or a: prints letter\r\n");
    printf("2)B or b: waits to get 4 letters and prints them\r\n");
    printf("3)C or c: see if buffer is circular\r\n");

	// TODO: Test your operating system!
    c = getc();
    switch (c)
    {
	case 'A':
	case 'a':
		printf("Case A:\n\r");
		putc(c);
		break;
	case 'B':
	case 'b':
		printf("Case B:\n\r");
		while(i < 4)
		{
			c = getc();
			putc(c);
			i++;
		}
		break;
	case 'C':
	case 'c':
		printf("Case C: Q is placed in putc() 1024 times from k = 0 to k = 1023. When k = 1024, the buffer should circulate back to serial_port[0] due to the modulus.\r\n");
		for(int k = 0; k <= 1024; k++)
		{
			if(k == 1024)
			{
				putc('W');
			}
			else
			{
				putc('\r\nQ');
			}
		}
		break;
    	default:
		printf("No test chosen.\r\n");
        	break;
    }

    while (numproc > 5)  // Four null procs plus this test proc.
    {
        resched();
    }

    printf("\r\n===TEST END===\r\n");
    printf("\r\n\r\nAll user processes have completed.\r\n\r\n");

    while (1);
    return;
}

void print_sem(semaphore sem)
{
    struct sement *semptr;
    semptr = &semtab[sem];

    printf("\r\n%d->state: %s\r\n", sem,
           (semptr->state == 0x01) ? "SFREE" : "SUSED");
    printf("%d->count: %d\r\n", sem, semptr->count);
    printf("%d->queue: %s\r\n\r\n", sem,
           (isempty(semptr->queue)) ? "EMPTY" : "NONEMPTY");
}
