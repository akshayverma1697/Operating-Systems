/**
 * @file 	mmu.c
 * @provides	mmu_section mmu_init
 *
 * Embedded Xinu, Copyright (C) 2008, 2018. All rights reserved.
 */

#include <mmu.h>

/**
 * Initializes the mmu to have virtual address == physical addresses,
 * also configures certain memory regions to be cacheable.
 */
void mmu_init()
{
    unsigned int ra;

    for (ra = 0;; ra += 0x00100000)
    {
        //mmu_section(ra, ra, 0x0 | 0x8);
        mmu_section(ra, ra, 0x15C06);
        if (ra >= 0x3F000000)
            break;              /* stop before IO peripherals, dont want cache on those... */
    }

    // peripherals
    for (;; ra += 0x00100000)
    {
        mmu_section(ra, ra, 0x0000);
        if (ra == 0xFFF00000)
            break;
    }

    start_mmu(MMUTABLEBASE);
}

/**
 * Create mmu section entry in the MMU Table.
 * @param vadd	virtual address
 * 		  padd	physical address
 * 		  flags	flags to mark that section of memory
 * @return always returns 0
 */
unsigned int mmu_section(unsigned int vadd, unsigned int padd,
                         unsigned int flags)
{
    unsigned int ra, rb, rc;

    ra = vadd >> 20;
    rb = MMUTABLEBASE | (ra << 2);
    rc = (padd & 0xFFF00000) | 0xC00 | flags | 2;
    PUT32(rb, rc);

    return 0;
}
