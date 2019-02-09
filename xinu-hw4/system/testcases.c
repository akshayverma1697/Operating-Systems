/**
 * @file testcases.c
 * @provides testcases
 *
 * Embedded XINU, Copyright (C) 2007, 2019.  All rights reserved.
 */

#include <xinu.h>

void print_lockent(int);

/**
 * Testcase functions.  These functions are used as workloads on other
 * cores during various testcases.
 */
void core_print(void)
{
    uint cpuid;
    cpuid = getcpuid();
    while (1)
        kprintf("Hello World from core %d\r\n", cpuid);
}

void core_acquire(spinlock_t lock)
{
    lock_acquire(lock);
    while (1)
        ;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c, i, status;
    spinlock_t testlock;

    kprintf("===TEST BEGIN===\r\n");

    c = kgetc();
    switch (c)
    {
    case '0':
        // 1 spinlock, create and acquire.
        // Tests to make sure lock field is being set.
        // Expected output is that lock field is set to SPINLOCK_LOCKED
        testlock = lock_create();

        lock_acquire(testlock);
        print_lockent(testlock);

        lock_free(testlock);
        break;

    case '1':
        // 1 spinlock, acquire and release
        // Tests to make sure lock field is being set correctly.
        // Expected output is that lock field is set to SPINLOCK_UNLOCKED.
        testlock = lock_create();

        lock_acquire(testlock);
        lock_release(testlock);
        print_lockent(testlock);

        lock_free(testlock);
        break;

    case '2':
        // Invalid spinlock test case.
        // An invalid number is sent to lock_acquire.
        // This will make sure students do error checking.
        // Expected output is SYSERR
        status = lock_acquire(-1);
        kprintf("status: %s\r\n", (status == SYSERR) ? "SYSERR" : "OK");
        break;

    case '3':
        // Invalid spinlock test case.
        // An invalid number is sent to lock_release.
        // Expected output is SYSERR.
        status = lock_release(-1);
        kprintf("status: %s\r\n", (status == SYSERR) ? "SYSERR" : "OK");
        break;

    case '4':
        // Acquire lock on another core.
        // This tests that the core field is being set.
        // Expected output is that lock field is locked and core field is 1.
        
        // TODO: Write this testcase.
        break;

    case '5':
        // Core competition test case.
        // Core 0 will acquire the lock, then unpark the other cores
        // which will try to also acquire the lock.
        //
        // The other cores should not be able to acquire the lock.. 
        // if they do then the lock is not working properly.
        //
        // Expected output is that the core field should be 0.

        // TODO: Write this testcase.
        lock_free(testlock);
        break;

    default:
        // Default testcase.
        // Output is non-deterministic.
        //
        // Unpark the cores and set them to print.
        // If output is coherent, then the serial lock is working.

        unparkcore(1, (void *)core_print, NULL);
        unparkcore(2, (void *)core_print, NULL);
        unparkcore(3, (void *)core_print, NULL);
    }


    kprintf("\r\n===TEST END===\r\n");
    return;
}

void print_lockent(int index)
{
    struct lockent *lentry;
    lentry = &locktab[index];

    kprintf("%d->state: %s\r\n", index,
            (lentry->state ==
             SPINLOCK_FREE) ? "SPINLOCK_FREE" : "SPINLOCK_USED");
    kprintf("%d->lock:  %s\r\n", index,
            (lentry->lock ==
             SPINLOCK_UNLOCKED) ? "SPINLOCK_UNLOCKED" :
            "SPINLOCK_LOCKED");
    kprintf("%d->core:  %d\r\n", index, lentry->core);
    kprintf("\r\n");
}
