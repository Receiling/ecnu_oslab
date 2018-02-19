#include <stdlib.h>
#include "counter.h"
#include "lock.h"

// counter_init
void counter_init(counter_t *c, int value) {
	c->value = value;
	lock_init(&c->lock, LOCK_TYPE);
}

// counter_get_value
int counter_get_value(counter_t *c) {
	lock_acquire(&c->lock, LOCK_TYPE);
	int result = c->value;
	lock_release(&c->lock, LOCK_TYPE);
	return result;
}

// counter_increment
void counter_increment(counter_t *c) {
	lock_acquire(&c->lock, LOCK_TYPE);
	c->value ++;
	lock_release(&c->lock, LOCK_TYPE);
}

// counter_decrement
void counter_decrement(counter_t *c) {
	lock_acquire(&c->lock, LOCK_TYPE);
	c->value --;
	lock_release(&c->lock, LOCK_TYPE);
}