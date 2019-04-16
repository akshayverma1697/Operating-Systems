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
    
    if((NULL == supertab) || (NULL == filetab)
        || (isbadfd(fd)) || (FILE_FREE == filetab[fd].fn_state))
    {
        return SYSERR;
    }
    
    
    if(FILE_DIRTY & filetab[fd].fn_state)
    {
        seek(DISK0, filetab[fd].fn_blocknum);
        write(DISK0, filetab[fd].fn_data, DISKBLOCKLEN);
        filetab[fd].fn_state &= ~FILE_DIRTY;
    }
    filetab[fd].fn_state &= ~FILE_OPEN;
    
    wait(supertab->sb_freelock);
    seek(DISK0, supertab->sb_dirlst->db_blocknum);
    signal(supertab->sb_dirlock);
    
    
    return OK;
}
