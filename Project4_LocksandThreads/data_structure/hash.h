#ifndef LIBHASH_H_INCLUDED
#define LIBHASH_H_INCLUDED

#include "list.h"

#define MAX_SIZE (int)2e7
#define HASH_FIX_THREADS 1

typedef struct __hash_t {
	int size;
	list_t lists[(int)MAX_SIZE];
} hash_t;

void hash_init(hash_t *hash, int size);
void hash_insert(hash_t *hash, unsigned int key);
void hash_delete(hash_t *hash, unsigned int key);
void *hash_lookup(hash_t *hash, unsigned int key);

#endif // LIBHASH_H_INCLUDED
