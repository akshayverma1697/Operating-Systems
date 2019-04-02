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
    while(curr->next != NULL)
    {
		struct memblk *blk;
     
		if(sizeof(memblk) == nbytes)
		{
			(*blk).field; 
			lock_release(memlock);
			return &memblk;
		}
		else if(sizeof(memblk) > nbytes)
		{
			prev = curr;
			curr = curr->next;
			curr->next =  leftover; //nbytes - sizeof(memblk)
		}
         
    }
    restore(im);
    return (void *)SYSERR;
}
