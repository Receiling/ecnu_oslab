#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <time.h>
#include "spinlock.h"
#include "mutex.h"
#include "twophase.h"

#define SPINLOCK 	1
#define MUTEX 		2
#define TWOPHASE 	3
#define PTHREAD		4
#define LOCK_TYPE 4


void lock_init(void* lock, int lock_type)
{
   switch (lock_type)
   {
   		case SPINLOCK:
   			spinlock_init((spinlock_t*)lock);
   			break;
   		case MUTEX:
   			mutex_init((mutex_t*)lock);
   			break;
   		case TWOPHASE:
   			twophase_init((twophase_t*)lock);
   			break;
   		case PTHREAD:
   			pthread_mutex_init((pthread_mutex_t*)lock, NULL);
   			break;
   		default:
   			break;
   }
   
}

void lock_acquire(void *lock, int lock_type)
{
    switch (lock_type)
   {
   		case SPINLOCK:
   			spinlock_acquire((spinlock_t*)lock);
   			break;
   		case MUTEX:
   			mutex_acquire((mutex_t*)lock);
   			break;
   		case TWOPHASE:
   			twophase_acquire((twophase_t*)lock);
   			break;
   		case PTHREAD:
   			pthread_mutex_lock((pthread_mutex_t*)lock);
   			break;
   		default:
   			break;
   }
}

void lock_release(void *lock, int lock_type)
{
    switch (lock_type)
   {
   		case SPINLOCK:
   			spinlock_release((spinlock_t*)lock);
   			break;
   		case MUTEX:
   			mutex_release((mutex_t*)lock);
   			break;
   		case TWOPHASE:
   			twophase_release((twophase_t*)lock);
   			break;
   		case PTHREAD:
   			pthread_mutex_unlock((pthread_mutex_t*)lock);
   			break;
   		default:
   			break;
   }
}