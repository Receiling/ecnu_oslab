#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <time.h>
#include "mutex.h"
#include "spinlock.h"
#include "twophase.h"
#include "lock.h"
#include "list.h"
#include "hash.h"

/*--------------------Test------------------------------*/
hash_t hash;

void *mythread(void* arg)
{
	printf("%s: begin\n", (char*)arg);
	int i = 0;
	for (; i < 1e6; i++) {
        hash_insert(&hash, (unsigned int)i);
	}
	for (; i < 1e6; i++) {
        hash_delete(&hash, (unsigned int)i);
	}
	printf("%s: done\n", (char*)arg);
	return NULL;
}

int main()
{
	pthread_t p1, p2;
	int rc;
	hash_init(&hash, 1);
	//printf("main: begin(counter = %d)\n", counter_get_value(&counter));
	rc = pthread_create(&p1, NULL, mythread, "A");
	rc = pthread_create(&p2, NULL, mythread, "B");

	rc = pthread_join(p1, NULL);
	rc = pthread_join(p2, NULL);
	//printf("main: done with both(counter = %d)\n", counter_get_value(&counter));
	return 0;
}
