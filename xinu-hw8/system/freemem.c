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

    //use ulong because top is a ulong//
    
	lock_acquire(memlock); // acquire memory lock
	prev = &freelist;
	next = freelist.next;
    
    while((next != NULL) && (block > next))
    {
		prev = next;
		next = next->next;
    }
    
    if(prev == &freelist)
    {
        top = NULL;//top is null if prev is the address of the freelist
    }
    else
    {
        top = (ulong)prev + prev->length;//getting top of prev memblock
    }
    
    if(((top > (ulong)block) || (next != NULL)) && ((((ulong)block + nbytes) > (ulong)next) || (((ulong)prev + prev->length) > block)))//make sure block isnt overlapping with prev 
    {
        restore(im);
        lock_release(memlock);
        return SYSERR;
    }
    
    freelist.length += nbytes;
    
    	//compacting
	if( ((ulong)block + nbytes) == (ulong)next)
    {
        block->next = next->next;
        block->length += block->length;
        next->next = next;
    }
    //compacting
    if( ((ulong)prev + prev->length) == (ulong)block)
    {
        prev->next = block->next;
        prev->length += block->length;
        block->next = block;
    }
    
	if(((ulong)block + block->length) == (ulong)next)
    {
        block->length += next->length;
        block->next = next->next;
    }
    

    
	lock_release(memlock);	
	restore(im);
    return OK;
}
