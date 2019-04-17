//TA-BOT:MAILTO akshay.verma@marquette.edu joshuah.solito@marquette.edu
/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
void printFreeList1(void);
//void createFiles(int);

command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.
    printf("This is where you should put some testing code.\n");
    
    char c; int i;
    
    if(nargs == 1) // if no arguments chosen
    {
            printf("No arguments chosen.\n");
            printf("Choose a number:\n");
            printf("1) Print Freelist\n");
            printf("2) Test Condition 1\n");
    }
    if(nargs == 2)
    {
           i = atoi(args[1]); 
    }
    if(nargs > 2)
    {
        printf("Too many arguments. Choose only one number\n");
    }
    
    switch(i)
    {
        case 1:
            printf("Printing freelist\n");
            printFreeList1();
            break;
        case 2:
            printf("Testing condition 1: creating a bunch of files and deleting them.\n");
            //createFiles(254);
            //char command[50];
            
            /*
            for(int i=0; i < 255; i++)
            {
                //sprintf(command, "cat > test%d.txt", i);
                system("cat > test.txt");
                printf("Created test%d.txt\n", i);
            }*/
            break;
        default:
            printf("No case chosen.\n");
            break;
    };
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

/*void createFiles(int count)
{
    FILE *a;
    char name[100];
    
    sprintf(name, "test%d
    .txt", count);
    
    a = fopen(name, "w");
    printf("Created %c\n", name);
    fclose();
    if(count > 0)
    {
        --count;
        createFiles(count);
    }
}*/


