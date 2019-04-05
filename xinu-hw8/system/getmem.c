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
	lock_acquire(memlock);//aqcuire memory lock
	prev = &freelist;//set prev to memory address of freelist
	curr = freelist.next;//set current to the adjacent memory block in the freelist
	//leftover = curr; //initial pointer to leftover
	while(curr != NULL)
	{
		if(curr->length == nbytes)//first comparisson--if memory block is the exact size as the memory to be allocated
		{
            //restructuring linked list of freelist
			prev->next = curr->next;//previous next links to the one after current
			freelist.length = freelist.length - nbytes; //get rid of allocated memory from freelist
			curr->next = curr;//remove current 
			lock_release(memlock);//release lock
			restore(im);//restore interrupts
			return (void *) curr;//return curr
		} 
		else if(curr->length > nbytes)//if memory block has more memory than what needs to be allocated (you will have leftovers)
		{
			leftover = (struct memblock *)((uint)curr + nbytes);//leftover has the size of curr + memory(nbytes)
			
			leftover->length = curr->length - nbytes;//the length of leftovers is the curr length minus what is being allocated
            curr->length = nbytes;//retrieve currents length
			
			leftover->next = curr->next;//set curr next to be after leftover
			
			prev->next = leftover;//link the adjacent block to prev to be leftover

			
			freelist.length = freelist.length - nbytes;
			
			curr->next = curr; //takes block out of freelist

			lock_release(memlock);
			restore(im);
			return (void *) curr;
		}
		curr = curr->next;
		prev = prev->next;
	}
	lock_release(memlock); // in case other cases don't work and the lock needs to be released
	restore(im);
	return (void *)SYSERR;
}
