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
	struct freeblock *freeblk, *free2;
	struct dirblock *swizzle;
	struct dentry *phw;
	int diskfd;
	struct disk *dsk;


	if(psuper == NULL)
	{
		return SYSERR;
	}

	phw = psuper->sb_disk;

	if(phw == NULL)
	{
		return SYSERR;
	}

	diskfd =	 phw - devtab;
	freeblk = psuper->sb_freelst;

	if(freeblk == NULL)
	{
		printf("Free List is empty\n");
		return SYSERR;
	}

	//guarantee mutually exclusive access to freelist
	wait(psuper->sb_freelock);

	while(freeblk->fr_next != NULL)
	{
		freeblk = freeblk->fr_next;//traverse freelist until it reaches the end++++
	}
	//first conditon see if there is enough space
	if(freeblk->fr_count < FREEBLOCKMAX)
	{
		freeblk->fr_free[freeblk->fr_count] = block;//puts block back into list?????
		freeblk->fr_count = freeblk->fr_count+1;

		seek(diskfd, freeblk->fr_blocknum);
		if(write(diskfd, freeblk, sizeof(struct freeblock))==SYSERR)
		{
			return SYSERR;
		}   
				//free2 
	}
	else//second condition if its greater than or equal
	{
		free2 = (struct freeblock*)malloc(sizeof(struct freeblock));//create new block of memory containing the same size as as freeblock
		free2->fr_blocknum = block;
		free2->fr_count = 0;//set amount of items to 0 because its a brand new block
		free2->fr_next = NULL;//nothing after free2 therefore null
		freeblock->fr_next = free2;//reset tail of freeblock
		//free2->fr_next = 0;	
		
		seek(diskfd, free2->fr_blocknum);
		if(write(diskfd, free2, sizeof(struct freeblock)) == SYSERR)
		{
			return SYSERR;
		}
	}
	if(psuper->sb_freelist == NULL)
	{
		freeblock = (struct freeblock*)malloc(sizeof(struct freeblock))//creates new freeblock
		freeblock->fr_blocknum = block;
		freeblock->fr_count = 0;//set items in the queue to 0 because its a new block
		freeblock->fr_next = NULL;//nothing after this block hence set it to NULL
		


		//Begin Swizzle// Make sure address of freeblock is saved as a block number
		seek(diskfd, block);
		if(write(diskfd, psuper, sizeof(struct freeblock)) == SYSERR)
		{
			return SYSERR;
		}
		
		swizzle = freeblock;
		dirlst = psuper->sb_dirlst;

		psuper->sb_dirlst = (struct freelst*)swizzle->db_blocknum;


		seek(diskfd, psuper->sb_blocknum);
		if(write(diskfd, psuper, sizeof(struct(freeblock))) == SYSERR)
		{
			return SYSERR;
		}
		psuper->sb_freelst = swizzle;
		psuper->dirlst = dirlst;
	}
signal(psuper ->sb_freelock);//free up semaphore	
return SYSERR;
} 
