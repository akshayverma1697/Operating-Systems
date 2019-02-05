//TA-BOT: MAILTO <joshuah.solito@marquette.edu>
//TA-BOT: MAILTO <akshay.verma@marquette.edu>
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

devcall putc(int dev, char c)
{
    return 0;
}


/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c = 0;

    kprintf("===TEST BEGIN===\r\n");
    
    while(!(isalpha(c)))
    {
        c = kgetc();
    }
    
    switch (c)
    {

        // TODO: Test your operating system!
        case 'A':
            kputc(c);
            break;
        case 'B':
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            kungetc(c);
            if(kungetc(c) == SYSERR)
            {
                kprintf("ERROR--GOOD");
            }
            else
            {
                kprintf("NO ERROR--BAD");
            }
            break;
        case 'C':
            kgetc('h');
            kgetc('e');
            kgetc('l');
            kgetc('l');
            kgetc('o');
        default:
            kprintf("Hello Xinu World!\r\n");
    }


    kprintf("\r\n===TEST END===\r\n");
    return;
}
