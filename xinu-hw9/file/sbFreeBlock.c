// TA-BOT:MAILTO joshuah.solito@marquette.edu akshay.verma@marquette.edu
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
    struct freeblock *free1, *free2, *free_swizz;
	struct dirblock  *dswizzle;
	struct dentry    *phw;
	int    i;
	int    diskfd;
	
    //error checking
    if ((block < 0) || (block > DISKBLOCKTOTAL))
    {
        return SYSERR;
    }
    
    if (NULL == psuper)
    {
		return SYSERR;
    }
    phw = psuper->sb_disk;
	if (NULL ==  phw)
    {
		return SYSERR;
    }
	
	diskfd  = phw - devtab;
	wait(psuper->sb_freelock);
	free1 = psuper->sb_freelst;
    
    
    //end of error checking

    
    if (free1 == NULL)//third condition if sb is pointing to null
	{
        free1 = (struct freeblock*)malloc(sizeof(struct freeblock));//create new block of memory containing the same size as as freeblock
		free1->fr_blocknum = block;
		free1->fr_count = 0;//set items in the queue to 0 because its a new block
		free1->fr_next = NULL;//nothing after this block hence set it to NULL
		psuper->sb_freelst = free1;
		seek(diskfd, block);
		if (SYSERR == write(diskfd, free1, sizeof(struct freeblock)))
        {
			return SYSERR;
        }
        //Begin Swizzle,  Make sure address of freeblock is saved as a block number
		free2 = free1;			
		dswizzle = psuper->sb_dirlst;
		psuper->sb_freelst = block;
		psuper->sb_dirlst = (struct freelst*)dswizzle->db_blocknum;//swizzle head
		seek(diskfd, psuper->sb_blocknum);

		if (SYSERR == write(diskfd, psuper, sizeof(struct superblock)))
        {
			return SYSERR;
        }
		psuper->sb_freelst = free2;
		psuper->sb_dirlst = dswizzle;
		signal(psuper->sb_freelock);//release lock
		return OK;
	}
	
 	//guarantee mutually exclusive access to freelist   
    while(free1->fr_next != NULL)
    {//traverse freelist until it reaches the end
        if(free1->fr_blocknum == block)
        {
            signal(psuper->sb_freelock);
            return SYSERR;
        }
        

        for(i = 0; i< free1->fr_count; i++)
        {
            if(free1->fr_free[i] == block)
            {
                signal(psuper->sb_freelock);
                return SYSERR;
            }
        }        
        free1 = free1->fr_next;
    }    
    
    if(free1->fr_blocknum == block)
        {
            signal(psuper->sb_freelock);
            return SYSERR;
        }
        
    for(i = 0; i< free1->fr_count; i++)
    {
        if(free1->fr_free[i] == block)
        {
            signal(psuper->sb_freelock);
            return SYSERR;
        }
    }        
        
    if(free1->fr_count < FREEBLOCKMAX)//Case 1: If there is space in the final accounting block  (freeblock->fr_count < FREEBLOCKMAX)
    {
        free1->fr_free[free1->fr_count] = block;
		free1->fr_count = free1->fr_count + 1; //increment count
	}
	else//second condition if its greater than or equal hence create new accounting block
	{
		free2 = (struct freeblock*)malloc(sizeof(struct freeblock));//create new block of memory containing the same size as as freeblock
		free2->fr_blocknum = block;
		free2->fr_count = 0;//set amount of items to 0 because its a brand new block
		free2->fr_next = NULL;//nothing after free2 therefore null
		free1->fr_next = free2;
		seek(diskfd, free2->fr_blocknum);
		if (write(diskfd, free2, sizeof(struct freeblock)) == SYSERR)
        {
			return SYSERR;
        }
	}
	
	free_swizz = free1->fr_next;

	if(free1->fr_next != NULL)
    {
		free1->fr_next = (struct freelst*)free_swizz->fr_blocknum;
    }
	seek(diskfd, free1->fr_blocknum);

	if(write(diskfd, free1, sizeof(struct freeblock)) == SYSERR)
    {
		return SYSERR;
    }
	free1->fr_next = free_swizz;
	signal(psuper->sb_freelock);//release lock
    return OK;
}
