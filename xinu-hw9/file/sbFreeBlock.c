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
    struct disk *dsk;

 
    if(NULL == psuper)
    {
        return SYSERR;
    }
    
    phw = psuper->sb_disk;
    
    if(NULL == phw)
    {
        return SYSERR;
    }
    
    //diskfd = phw - ftab;
    freeblk = psuper->sb_freelst;
    
    if(NULL == freeblk)
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
        freeblk->fr_count++;
        freeblk->fr_free[freeblk->fr_count] = block;//puts block back into list?????
        
        //free2 
    }
        
     /*   
    //if it is greater than or equal to
    if(freeblk->fr_count >= FREEBLOCKMAX)//after 55 blocks have been allocated
    {
        fr_count = 0;//free all space (nuke the freelist)???
        freeblk->fr_free[freeblk->fr_count] = block;//use all that space for block????
    }
    
    //if supertab->sb_freelst = null
    if(psuper->sb_freelst == NULL)//after all blocks have been used
    {
        psuper->sb_freelst = freeblk;//I DONT KNOW WHAT TO DO
    }
    */
    return SYSERR;
}
