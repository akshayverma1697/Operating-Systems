/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
void printFreeList1(void);
command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.
    printf("This is where you should put some testing code.\n");
    return OK;
}


void printFreeList1(void)
{
    int i;
    struct freeblock *fb = NULL;
    if ((NULL == supertab) || (NULL == (fb = supertab->sb_freelst)))
    {
        printf("Freelst: null\n");
        return;
    }

    printf("Freelst collector nodes:\n");
    while (fb)
    {
        printf("Blk %3d, cnt %3d = ", fb->fr_blocknum, fb->fr_count);
        for (i = 0; (i < fb->fr_count) && (i < 10); i++)
        {
            printf("[%03d]", fb->fr_free[i]);
        }
        if (fb->fr_count >= 10)
        {
            printf("...[%03d]", fb->fr_free[fb->fr_count - 1]);
        }
        printf("\n");
        fb = fb->fr_next;
    }
}


