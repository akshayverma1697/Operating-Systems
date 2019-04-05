// TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, uint nbytes)
{
    register struct memblock *block, *next, *prev;
    irqmask im;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < (ulong)memheap)
        || ((ulong)memptr > (ulong)platform.maxaddr))
    {
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

	/* TODO:
     *      - Acquire memory lock (memlock)
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     */

	lock_acquire(memlock); // acquire memory lock
	prev = &freelist;
	next = freelist.next;
    while(next != NULL)
    {
		if(block < next) // find where memory block should be placed back
		{
			if(prev < block) // prev < block < curr
			{
				top = prev->length + nbytes;//retrive top of previous memblock
				if(top != prev->length)
				{
					if(top != next->length)
					{
						prev->next = block;
						block->next = next;
						if( ((uint)block + nbytes) == (uint)next - 1)// shift starting address of block by 1
						{
							block->next = next->next;
							block->length += block->length;
							next->next = next;
						}
						if( ((uint)prev + prev->length) == (uint)block -1)
						{
							prev->next = block->next;
							prev->length += block->length;
							block->next = block;
						}
						lock_release(memlock);
						restore(im);
						return OK;
					}
				}
			}
		}
		next = next->next;
		prev = next;
    }
	lock_release(memlock);	
	restore(im);
    return OK;
}
