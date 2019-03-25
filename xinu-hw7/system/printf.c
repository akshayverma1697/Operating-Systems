/**
 * @file printf.c
 */
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
