#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED

typedef struct MUTEX_T
{
    int flag;
    int guard;
    int tag;
}mutex_t;

void mutex_init(mutex_t* lock);
void mutex_acquire(mutex_t *lock);
void mutex_release(mutex_t *lock);

#endif // MUTEX_H_INCLUDED
