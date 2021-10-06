#ifndef _SPIN_BARRIER_H
#define _SPIN_BARRIER_H

#include <pthread.h>
#include <iostream>

struct spin_barrier {
    //volatile int arrive;
    //volatile int leave;
    int counter;
    int total;
    volatile int flag;
    pthread_spinlock_t lock;
};

void spin_barrier_init(spin_barrier* barrier, int nThread);
void spin_barrier_wait(spin_barrier* barrier, int* local);
void spin_barrier_destroy(spin_barrier* barrier);
#endif
