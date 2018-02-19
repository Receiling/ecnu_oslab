#ifndef LIBLIST_H_INCLUDED
#define LIBLIST_H_INCLUDED

#include "lock.h"
// structure of node
typedef struct __node_t {
	int key;
	struct __node_t *next;
} node_t;

// structure of list
typedef struct __list_t {
	node_t *head;
	#if LOCK_TYPE == SPINLOCK
		spinlock_t lock;
	#elif LOCK_TYPE == MUTEX
		mutex_t lock;
	#elif LOCK_TYPE == TWOPHASE
		twophase_t lock;
	#elif LOCK_TYPE == PTHREAD
		pthread_mutex_t lock;
	#endif
} list_t;

void list_init(list_t *list);
void list_insert(list_t *list, unsigned int key);
void list_delete(list_t *list, unsigned int key);
void *list_lookup(list_t *list, unsigned int key);
void * list_dump(list_t *list);
#endif // LIBLIST_H_INCLUDED
