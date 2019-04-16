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
    struct freeblock *freeblk;
    
    diskfd = phw - ftab;
    freeblk = psupe->sb_freelst;
    
    
    wait(sem freeblock);
    
    //first conditon add if there is enough space
    if(freeblk->fr_count < FREEBLOCKMAX)
    {
        freeblk->fr_count++;
    }
        
        
    //if it is greater than or equal to
        
    
    //if supertab->sb_freelst = null
    return SYSERR;
}
