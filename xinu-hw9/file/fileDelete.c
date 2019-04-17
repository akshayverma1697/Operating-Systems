/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.
    
    if((NULL == supertab) || (NULL == filetab))
    {
        return SYSERR;
    }
    
    wait(supertab->sb_dirlock);
    
    sbFreeBlock(supertab, supertab->sb_dirlst->db_fnodes[fd].fn_blocknum);
	supertab->sb_dirlst->db_fnodes[fd].fn_state = FILE_FREE;
    seek(DISK0, filetab[fd].fn_blocknum);
    write(DISK0, filetab[fd].fn_data, DISKBLOCKLEN);
    
    signal(supertab->sd_dirlock);
    
    
    return OK;
}
