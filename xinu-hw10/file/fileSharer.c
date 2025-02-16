/*TA-BOT:MAILTO akshay.verma@marquette.edu joshuah.solito@marquette.edu */
/* fileSharer.c - fileSharer */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <kernel.h>
#include <xc.h>
#include <file.h>
#include <fileshare.h>
#include <ether.h>
#include <network.h>
#include <nvram.h>

struct fishnode school[SCHOOLMAX];
char   fishlist[DIRENTRIES][FNAMLEN];

static uchar bcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uchar myMAC[ETH_ADDR_LEN];

int fishAnnounce(uchar *packet)
{
	struct ethergram *eg = (struct ethergram *)packet;
	int i = 0;

	for (i = 0; i < SCHOOLMAX; i++)
	{
		/* Check to see if node already known. */
		if (school[i].used && 
			(0 == memcmp(school[i].mac, eg->src, ETH_ADDR_LEN)))
			return OK;
	}
	for (i = 0; i < SCHOOLMAX; i++)
	{
		/* Else find an unused slot and store it. */
		if (!school[i].used)
		{
			school[i].used = 1;
			memcpy(school[i].mac, eg->src, ETH_ADDR_LEN);
			memcpy(school[i].name, eg->data + 1, FISH_MAXNAME);
			//printf("Node %s\n", school[i].name);
			return OK;
		}
	}
	return SYSERR;
}
/*------------------------------------------------------------------------
 * fishPing - Reply to a broadcast FISH request.
 *------------------------------------------------------------------------
 */
void fishPing(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;

	/* Source of request becomes destination of reply. */
	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	/* Source of reply becomes me. */
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	/* Zero out payload. */
	bzero(eg->data, ETHER_MINPAYLOAD);
	/* FISH type becomes ANNOUNCE. */
	eg->data[0] = FISH_ANNOUNCE;
	strncpy(&eg->data[1], nvramGet("hostname\0"), FISH_MAXNAME-1);
	write(ETH0, packet, ETHER_SIZE + ETHER_MINPAYLOAD);
}


/*------------------------------------------------------------------------
 * fishDirask - Reply to a unicast FISH request.
 *------------------------------------------------------------------------
 */
void fishDirask(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	int i = 0;
	int k = 0; //i and k will be used as counts
	int PPKTsize = 0; // used to see the size of our packet

	/* Source of request becomes destination of reply. */
	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	/* Source of reply becomes me. */
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	/* Zero out payload. */
	bzero(eg->data, ETHER_MINPAYLOAD);
	
	ppkt = ppkt +  ETHER_SIZE;//move ppkt to current location

	/* FISH type becomes DIRLIST. */
	eg->data[0] = FISH_DIRLIST;
	
	ppkt = ppkt+1;//adjust location


	for(i=0; i<DIRENTRIES;i++)//locating files based on the node 
	{
		ppkt += FNAMLEN;
		if(supertab->sb_dirlst->db_fnodes[i].fn_state)//check to see if the node has a file
		{
			strncpy(&eg->data[i*FNAMLEN+1], supertab->sb_dirlst->db_fnodes[i].fn_name, FNAMLEN);//saving the name of the File in the memory address of eg->data 
		}
		else//if the node doesnt have a file we know the rest of the nodes wont have a file so fill them with zero's
		{
			for(k=1; k < FNAMLEN; k++ )//start from the index after the last spot that had a file 
			{
				eg->data[i*FNAMLEN + k] = 0;//begin zeroing the rest out
			}
		}
	}
	
//	should we make a case check to see if the packet is larger than the MINPAYLOAD
	
	write(ETH0, packet, ETHER_SIZE + DIRENTRIES * FNAMLEN);
}

/*-----------------------------------------------------------------------
 *fishDirList - Print all the files available to be shared
 *-----------------------------------------------------------------------
 */ 
int fishDirList(uchar *packet)
{
	//uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	printf("Printing all files able to be shared\n");
	//int i = 0;
	for(int i = 0; i<DIRENTRIES; i++)
	{
		strncpy(fishlist[i], &eg->data[i*FNAMLEN+1], FNAMLEN);//copy all of the packet data and saving it into fishlist to be printed
		//this will be used because it will print all the files available to be shared
	}
	
	return OK;

}

/*-----------------------------------------------------------------------
 *fishGetFile--
 * ----------------------------------------------------------------------
 */
void fishGetFile(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	

	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	bzero(&eg->data[1+FNAMLEN], ETHER_MINPAYLOAD);
    printf("Entered fishgetFile\n");
    printf("this is eg data onne %s\n", &eg->data[1]);
    

	int fileFOUND = 0; //0 refers to FALSE
    int j = 0;
	int i = 0; //counter
	int packetLength;
	char tempFile;
	for(i = 0; i < DIRENTRIES; i++)
	{
		if(supertab->sb_dirlst->db_fnodes[i].fn_state == FILE_USED)
		{
			if(strncmp(supertab->sb_dirlst->db_fnodes[i].fn_name, &eg->data[1], FNAMLEN) == 0)
			{
                printf("FILE is FOUND\n");
				fileFOUND = 1;//set to TRUE if it is able to get in the if statement that checks if they are equivalent
				eg->data[0]=FISH_HAVEFILE;
                ppkt++;
                ppkt = ppkt + FNAMLEN;
				
                int fd = fileOpen(&eg->data[1]);
                while(j<DISKBLOCKLEN)
                {
                    if((tempFile = fileGetChar(fd)) != SYSERR)
                    {
                        eg->data[(FNAMLEN+1+j)] = tempFile;
                    }
                    else
                    {
                        eg->data[(FNAMLEN+1+j)] = 0;
                    }
                    j++;
                }
                fileClose(fd);
                ppkt = ppkt + DISKBLOCKLEN;
                break;
			}
		}
	}
	if(fileFOUND == 0)
    {
        eg->data[0]=FISH_NOFILE;
        ppkt++;
        ppkt = ppkt + FNAMLEN;
    }
    packetLength = ETHER_SIZE + ETHER_MINPAYLOAD;
    if((ppkt-packet)>(ETHER_SIZE + ETHER_MINPAYLOAD))
    {
        packetLength = (ppkt-packet);
    }

	write(ETH0, packet, packetLength);
}

/*----------------------------------------------------------------------
 *fishHaveFile
 *----------------------------------------------------------------------
 */ 
void fishHaveFile(uchar *packet)
{
    struct ethergram *eg = (struct ethergram *)packet;
	int sizeOfPacket = DISKBLOCKLEN + FNAMLEN + 1;
	int i;

	int c;
	char tempFile[FNAMLEN + 1];
	bzero(tempFile, FNAMLEN+1);
    strncpy(tempFile, &eg->data[1], FNAMLEN);
    //tempFile = &packet;
	
	if((c = fileOpen((char *) tempFile)) != SYSERR)
	{
		if(fileDelete(c) == SYSERR)
		{
			printf("File taken\n");
			return;
		}
		//printf("File not deleted?\n"); // check
	}
	
	if((c = fileCreate((char *) tempFile)) != SYSERR)
	{
		for(i = FNAMLEN+1; i<DISKBLOCKLEN+FNAMLEN+1; i++)
		{
			filePutChar(c,eg->data[i]); 
		}
		fileClose(c);
		printf("Here is the file\n");
        return OK;
	}
	else
	{
		printf("FAILURE--NO FILE");
	}
}

/*-----------------------------------------------------------------------
 *fishNoFile - No files
 * ----------------------------------------------------------------------
 */
void fishNoFile(uchar *packet)
{
	struct ethergram *eg = (struct ethergram *)packet;
	printf("Unable to find file \n"); //if this case is entered we print that no files can be found
}

/*------------------------------------------------------------------------
 * fileSharer - Process that shares files over the network.
 *------------------------------------------------------------------------
 */
int fileSharer(int dev)
{
	uchar packet[PKTSZ];
	int size;
	struct ethergram *eg = (struct ethergram *)packet;

	enable();
	/* Zero out the packet buffer. */
	bzero(packet, PKTSZ);
	bzero(school, sizeof(school));
	bzero(fishlist, sizeof(fishlist));

	/* Lookup canonical MAC in NVRAM, and store in ether struct */
 	colon2mac(nvramGet("et0macaddr"), myMAC);

	while (SYSERR != (size = read(dev, packet, PKTSZ)))
	{
		/* Check packet to see if fileshare type with
		   destination broadcast or me. */
		if ((ntohs(eg->type) == ETYPE_FISH)
			&& ((0 == memcmp(eg->dst, bcast, ETH_ADDR_LEN))
				|| (0 == memcmp(eg->dst, myMAC, ETH_ADDR_LEN))))
		{
			switch (eg->data[0])
			{
			case FISH_ANNOUNCE:
				fishAnnounce(packet);
				break;

			case FISH_PING:
				fishPing(packet);
				break;

		// TODO: All of the cases below.

			case FISH_DIRASK:
				fishDirask(packet);
				break;
			case FISH_DIRLIST:
				fishDirList(packet);
				break;
			case FISH_GETFILE:
				fishGetFile(packet);
				break;
			case FISH_HAVEFILE:
				fishHaveFile(packet);
				break;
			case FISH_NOFILE:
				fishNoFile(packet);
				break;
			default:
				printf("ERROR: Got unhandled FISH type %d\n", eg->data[0]);
			}
		}
	}

	return OK;
}
