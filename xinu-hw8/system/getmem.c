// TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(uint nbytes)
{
    register memblk *prev, *curr, *leftover;
    irqmask im;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

	/* TODO:
     *      - Acquire memory lock (memlock)
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with simple compaction)
     *      - Release memory lock
     *      - return memory address if successful
     */
    lock_acquire(memlock);
	prev = &freelist;
	curr = freelist.next;
	leftover = curr;
	//struct memblk *blk;
    while(curr->next != NULL)
    {
		if(curr->length == nbytes)
		{
			curr->length = nbytes; //(*blk).field; 
			lock_release(memlock);
			return curr;
		}
		else if(curr->length > nbytes)
		{
			prev->length = curr->length;
			curr = curr->next;
			leftover->length = curr->length - nbytes;
			curr->next =  leftover; //nbytes - sizeof(memblk)
			lock_release(memlock);
			return curr;
		}
    }
    restore(im);
    return (void *)SYSERR;
}
