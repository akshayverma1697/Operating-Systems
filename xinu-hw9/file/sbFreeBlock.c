//TA-BOT: MAILTO akshay.verma@marquette.edu joshuah.solito@marquette.edu
/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
	// TODO: Add the block back into the filesystem's list of
	//  free blocks.  Use the superblock's locks to guarantee
	//  mutually exclusive access to the free list, and write
	//  the changed free list segment(s) back to disk.
	struct freeblock *freeblock, *free2;
	struct dirblock *swizzle;
	struct dentry *phw;
	struct disk *dsk;
    int diskfd;
    
	if(psuper == NULL)
	{
		return SYSERR;
	}
	phw = psuper->sb_disk;

	if(phw == NULL)
	{
		return SYSERR;
	}

	diskfd = phw - devtab;
	freeblock = psuper->sb_freelst;

	//guarantee mutually exclusive access to freelist
	wait(psuper->sb_freelock);

	while(freeblock->fr_next != NULL)
	{
		freeblock = freeblock->fr_next;//traverse freelist until it reaches the end
	}
    
    if(psuper->sb_freelst == NULL)//third condition if sb is pointing to null

	{
		freeblock = (struct freeblock*)malloc(sizeof(struct freeblock));//creates new freeblock
		freeblock->fr_blocknum = block;
		freeblock->fr_count = 0;//set items in the queue to 0 because its a new block
		freeblock->fr_next = NULL;//nothing after this block hence set it to NULL

		//Begin Swizzle// Make sure address of freeblock is saved as a block number
		seek(diskfd, block);
		if(write(diskfd, psuper, sizeof(struct freeblock)) == SYSERR)
		{
			return SYSERR;
		}
		
		swizzle = psuper->sb_dirlst;

		psuper->sb_dirlst = (struct freelst*)swizzle->db_blocknum;//head

		seek(diskfd, psuper->sb_blocknum);
		if(write(diskfd, psuper, sizeof(struct freeblock)) == SYSERR)
		{
			return SYSERR;
		}
		psuper->sb_dirlst = swizzle;
	}
	
    
    if(freeblock->fr_count < FREEBLOCKMAX)//first conditon see if there is enough space
	{
		freeblock->fr_free[freeblock->fr_count] = block;//puts block back into list?????
		freeblock->fr_count = freeblock->fr_count+1;

		seek(diskfd, freeblock->fr_blocknum);
		if(write(diskfd, freeblock, sizeof(struct freeblock))==SYSERR)
		{
			return SYSERR;
		}   
	}
    else if(freeblock->fr_count >= FREEBLOCKMAX)//second condition if its greater than or equal hence create new accounting block
	{
		free2 = (struct freeblock*)malloc(sizeof(struct freeblock));//create new block of memory containing the same size as as freeblock
		free2->fr_blocknum = block;
		free2->fr_count = 0;//set amount of items to 0 because its a brand new block
		free2->fr_next = NULL;//nothing after free2 therefore null
		freeblock->fr_next = free2;//reset tail of freeblock
		//free2->fr_next = 0;	
		
		seek(diskfd, freeblock->fr_blocknum);
		if(write(diskfd, freeblock, sizeof(struct freeblock)) == SYSERR)
		{
			return SYSERR;
		}
	}



	

signal(psuper ->sb_freelock);//free up semaphore	
return OK;
} 
