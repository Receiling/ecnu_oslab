#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <time.h>
#include "twophase.h"

static long sys_futex(void *addr1, int op, int val1, struct timespec *timeout, void *addr2, int val3)
{
	return syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}

static inline uint xchg(volatile unsigned int *addr, unsigned int newval)
{
    uint result;
    asm volatile("lock; xchgl %0, %1" : "+m" (*addr), "=a" (result) : "1" (newval) : "cc");
    return result;
}

static inline void atomic_add(twophase_t* lock, int value)
{
    while(xchg(&(lock -> guard), 1) == 1);
    lock -> tag += value;
    lock -> guard = 0;
}

static inline void atomic_minus(twophase_t* lock, int value)
{
    while(xchg(&(lock -> guard), 1) == 1);
    lock -> tag -= value;
    lock -> guard = 0;
}

void twophase_init(twophase_t* lock)
{
    lock -> flag = 0;
    lock -> guard = 0;
    lock -> tag = 0;
}

void twophase_acquire(twophase_t *lock)
{
    if(xchg(&(lock -> flag), 1) == 0) return;

    atomic_add(lock, 1);
    while(1)
    {
        if(xchg(&(lock -> flag), 1) == 0)
        {
            atomic_minus(lock, 1);
            return;
        }

        int cnt = SPIN_TIMES;
        while(cnt--)
        {
            if(lock -> flag == 0) break;
        }
        if(lock -> flag == 0) continue;

        sys_futex(&(lock -> flag), FUTEX_WAIT, 1, 0, 0, 0);
    }
}

void twophase_release(twophase_t *lock)
{
    lock -> flag = 0;

    if(lock -> tag != 0)
    {
        sys_futex(&(lock -> flag), FUTEX_WAKE, 1, 0, 0, 0);
    }
}