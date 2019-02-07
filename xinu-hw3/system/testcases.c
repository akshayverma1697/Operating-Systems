//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
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
    
    while(c==0)
    {
        c = kgetc();
    }
    
    switch (c)
    {

        // TODO: Test your operating system!
        case 'A':
            kprintf("Running putc command:\r\n");
            kputc(c);
            break;
        case 'B':
            //Check to see if you get error from running unget method to many times
            kprintf("Running unget method:\r\n");
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
            if(kungetc(c) == SYSERR)//11>10 so it should return Error
            {
                kprintf("ERROR--GOOD\r\n");
            }
            else
            {
                kprintf("NO ERROR--BAD\r\n");
            }
            break;
        case 'C':
            //Check to see if the unget method runs single time--no error
            kprintf("Running unget method:\r\n");
            if(kungetc(c)==SYSERR)
            {
                kprintf("ERROR--BAD\r\n");
            }
            else
            {
                kprintf("NO ERROR--GOOD\r\n");
            }
            break;
        case 'D':
            //check to see if the checkc method will return True
            kprintf("Running checkc method\r\n");
            if(kcheckc()==1)
            {
                kprintf("TRUE\r\n");
            }
            else
            {
                kprintf("FALSE\r\n");
            }
            break;
        case 'E':
            //Should print H
            kprintf("Running ungetc method for a single character:\r\n");
            kungetc('H');
            kputc(kgetc());
            while(kcheckc()==1)
            {
                kputc(kgetc());
            }
            if(kcheckc()==0)
            {
                kprintf("\r\nNo characters left--GOOD");
            }
            else
            {   
                kprintf("\r\ncharacters available--BAD");
            }
            kprintf("\r\n");
            break;
        case 'F':
            //Should print Hello backwards
            kungetc('H');
            kungetc('E');
            kungetc('L');
            kungetc('L');
            kungetc('O');
            while(kcheckc()==1)
            {
                kputc(kgetc());
            }
            kprintf("\r\n");
            break;
        case 'G':
            kungetc('O');
            kungetc('L');
            kungetc('L');
            kungetc('E');
            kungetc('H');
            if(kcheckc()==1)
            {
                kprintf("characters ready, printing: \n\r");
            }
            while(kcheckc()==1)
            {
                kputc(kgetc());
            }
            if(kcheckc()==0)
            {
                kprintf("\r\nNo more characters available\r\n");
            }
            break;
        default:
            kprintf("Hello Xinu World!\r\n");
    }


    kprintf("\r\n===TEST END===\r\n");
    return;
}
