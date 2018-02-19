#include <stdlib.h>
#include "hash.h"
#include "list.h"

// hash_init
void hash_init(hash_t *hash, int size) {
	hash->size = size;
	int i;
	for (i = 0; i < size; i++)
		list_init(&(hash->lists[i]));
}

// hash_insert
void hash_insert(hash_t *hash, unsigned int key) {
	int bucket = key % hash->size;
	list_insert(&(hash->lists[bucket]), key);
}

// hash_delete
void hash_delete(hash_t *hash, unsigned int key) {
	int bucket = key % hash->size;
	list_delete(&(hash->lists[bucket]), key);
}

// hash_lookup
void *hash_lookup(hash_t *hash, unsigned int key) {
	int bucket = key % hash->size;
	return list_lookup(&(hash->lists[bucket]), key);
}