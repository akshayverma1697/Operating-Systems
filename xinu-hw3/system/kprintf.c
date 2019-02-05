//TA-BOT: MAILTO <akshay.verma@marquette.edu>//
//TA-BOT: MAILTO <joshuah.solito@marquette.edu>//
/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];
int unbuf = 0; // index for ungetArray (current empty space)

/**
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(void)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    // TODO: First, check the unget buffer for a character.
    //       Otherwise, check UART flags register, and
    //       once the receiver is not empty, get character c.

	// JOSH
    // on assignment website, kungetc should hold K&R's getch() and ungetch() functions
	// might have to transfer this
	if(unbuf > 0) // if no character stored in array
	{
        return (int) ungetArray[unbuf--];
        
    }
    else
    {
		while(regptr->fr & PL011_FR_RXFE) // wait while receive flag is empty
        {
                ;
        }
        return (int) regptr->dr; // return character in data register
	}
    return SYSERR;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    // TODO: Check the unget buffer and the UART for characters.
    
    
    if((regptr->fr & PL011_FR_BUSY) || (unbuf > 0)) //This should check if a character is available in UART//
    {
        return 1;
    }
    else
    {
        return 0;
    }
    

    return SYSERR;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
    // TODO: Check for room in unget buffer, put the character in or discard.
	if(unbuf < UNGETMAX) // if index is less then array max size
	{
		ungetArray[unbuf] = c;// add c to ungetArray and increment unbuf by 1
		unbuf++;
	}

    return SYSERR;
}


/**
 * Synchronously write a character to a UART.  This blocks until the character
 * has been written to the hardware.  The interrupt handler is not used.
 *
 * @param c
 *      The character to write.
 *e
 * @return
 *      The character written to the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */

syscall kputc(uchar c)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    // TODO: Check UART flags register.
    //       Once the Transmitter FIFO is not full, send character c.
    
    while(regptr->fr & PL011_FR_TXFF)
    {
        ;
    }
    
    regptr->dr = c;//register pointer goes to data regiser and sends 'c'//
    
    return SYSERR;
}

/**
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(int, int))kputc, 0);
    va_end(ap);
    return retval;
}
