/**
 * @file printf.c
 */
//TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu//
/* Embedded Xinu, Copyright (C) 2018. All rights reserved. */

#include <xinu.h>

extern struct uart serial_port;


/**
 * Asynchrounously read a character from the UART.
 *
 * @return
 *      The character read from the UART.
 */
uchar getc(void)
{
	uchar c;
	irqmask im = disable();

	/* TODO:
	 * Asynchronously read a character from the UART.
	 * Wait for input using the appropriate semaphore.
	 * Store the first byte in the input buffer (as a character) to later be returned.
	 * Then, decrement the counter of bytes in the input buffer.
	 * Increment the index of the input buffer's first byte
	 * with respect to the total length of the UART input buffer.
	 */
    c = ((struct pl011_uart_csreg *) serial_port.csr)->dr;
    wait(serial_port.isema);
    serial_port.in[serial_port.istart] = c;// use istart to store the first byte in input buffer uchar c is your character 
    serial_port.icount--;//decrement icount
    serial_port.in[(serial_port.istart+serial_port.icount)%UART_IBLEN];//increment index with respect to UART input buffer use modulus to make circular
	restore(im);
	return c;
}


/**
 * Asynchronously write a character to the UART.
 *
 * @param c
 *      The character to write.
 *
 * @return OK on success
 */
syscall putc(char c)
{
	irqmask im = disable();
	
	/* TODO:
	 * First, check if the UART transmitter is idle (see include/uart.h for UART control block information).
	 * If it is idle, set its state to busy (hint: not idle), then write the character to the
	 * UART control and status register's data register.
	 * If it is not idle, use the appropriate UART semaphore to wait for the UART's buffer space to be free.
	 * Next, guard the following operation with a spin lock:
	 * Place the character into the UART output buffer with respect to the index of the buffer's first byte,
	 * amount of bytes in buffer, and total length of the UART output buffer.
	 * Then, increment the counter of bytes in the output buffer. Release the spinlock.
	 */
    
    if(serial_port.oidle)//checking to see if serial port is idle
    {//enter if statement 
        serial_port.oidle = FALSE;// set serial port to not idle
        ((struct pl011_uart_csreg *) serial_port.csr)->dr = c;//write character to the data register
    }
    else//if not idle
    {
        wait(serial_port.osema);//wait on count, for input bytes to be ready
        lock_acquire(serial_port.olock);
        serial_port.out[(serial_port.ostart+serial_port.ocount)%UART_OBLEN] = c;// set c equal to ostart(index to the buffers first byte) divided by BUFFLEN --making it circular
        serial_port.ostart++;//increment ostart
        lock_release(serial_port.olock);//release the spin lock
    }

	restore(im);
	return OK;
}


/**
 * printf: formatted, asynchronous output to UART device.
 *
 * @param format
 *      The format string.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall printf(const char *format, ...)
{
	int retval;

	va_list ap;
	va_start(ap, format);
	retval = _doprnt(format, ap, (int (*)(int, int))putc, 0);
	va_end(ap);

	return retval;
}
