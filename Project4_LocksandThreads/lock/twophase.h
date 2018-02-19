#ifndef TWOPHASE_H_INCLUDED
#define TWOPHASE_H_INCLUDED

#define SPIN_TIMES 105
typedef struct TWOPHASE_T
{
    int flag;
    int guard;
    int tag;
}twophase_t;

void twophase_init(twophase_t* lock);
void twophase_acquire(twophase_t *lock);
void twophase_release(twophase_t *lock);

#endif // TWOPHASE_H_INCLUDED
