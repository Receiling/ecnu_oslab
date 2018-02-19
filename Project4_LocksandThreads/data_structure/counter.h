#ifndef LIBCOUNTER_H_INCLUDED
#define LIBCOUNTER_H_INCLUDED

#include "lock.h"

typedef struct __counter_t {
	int value;
	#if LOCK_TYPE == SPINLOCK
		spinlock_t lock;
	#elif LOCK_TYPE == MUTEX
		mutex_t lock;
	#elif LOCK_TYPE == TWOPHASE
		twophase_t lock;
	#elif LOCK_TYPE == PTHREAD
		pthread_mutex_t lock;
	#endif
} counter_t;

void counter_init(counter_t *c, int value);
int counter_get_value(counter_t *c);
void counter_increment(counter_t *c);
void counter_decrement(counter_t *c);

#endif // LIBCOUNTER_H_INCLUDED