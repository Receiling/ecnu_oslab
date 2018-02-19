#ifndef SPINLOCK_H_INCLUDED
#define SPINLOCK_H_INCLUDED

typedef struct SPINLOCK_T
{
    int flag;
}spinlock_t;

void spinlock_init(spinlock_t* lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);


#endif // SPINLOCK_H_INCLUDED
