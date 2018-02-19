#ifndef LOCK_H_INCLUDED
#define LOCK_H_INCLUDED

#include "spinlock.h"
#include "mutex.h"
#include "twophase.h"
#include <pthread.h>

#define SPINLOCK 	1
#define MUTEX 		2
#define TWOPHASE 	3
#define PTHREAD		4
#define LOCK_TYPE 3

void lock_init(void* lock, int lock_type);
void lock_acquire(void *lock, int lock_type);
void lock_release(void *lock, int lock_type);

#endif // LOCK_H_INCLUDED
