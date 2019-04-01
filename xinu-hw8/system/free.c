/**
 * @file free.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup libxc
 *
 * Attempt to free a block of memory based on malloc() accounting information
 * stored in preceding two words.
 *
 * @param ptr
 *      A pointer to the memory block to free.
 */
syscall free(void *ptr)
{
    struct memblock *block;

	/* TODO:
     *      1) set block to point to memblock to be free'd (ptr)
     *      2) find accounting information of the memblock
     *      3) call freemem on the block with its length
     */

    return OK;
}
