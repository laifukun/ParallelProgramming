#include "selection.h"
#include <spin_barrier.h>
#include <iostream>

/************************
 * Your code here...    *
 * or wherever you like *
 ************************/

void spin_barrier_init(spin_barrier* barrier, int nThread) {
    barrier->total = nThread;
    //barrier->arrive = 0;
    //barrier->leave = nThread;
    barrier->counter = 0;
    pthread_spin_init(&barrier->lock, 0);

    barrier->flag = 0;
}
void spin_barrier_wait(spin_barrier* barrier, int* local) {

/*
    pthread_spin_lock(&barrier->lock);
    if (barrier->arrive == 0) {
        pthread_spin_unlock(&barrier->lock);
        while(barrier->leave != barrier->total);
        pthread_spin_lock(&barrier->lock);
        barrier->flag = 0;
    }
    barrier->arrive++;
    
    if (barrier->arrive == barrier->total) {
        barrier->arrive = 0;
        barrier->leave = 0;
        barrier->flag = 1;
    }    
    pthread_spin_unlock(&barrier->lock);

    while(barrier->flag == 0);
    pthread_spin_lock(&barrier->lock);
    barrier->leave++;
    pthread_spin_unlock(&barrier->lock);
*/

    *local = 1 - *local;
    
    pthread_spin_lock(&barrier->lock);
    barrier->counter++;
    int arrived = barrier->counter;
    if (arrived == barrier->total) {
        pthread_spin_unlock(&barrier->lock);
        barrier->counter = 0;
        barrier->flag = *local;
    } else {
        pthread_spin_unlock(&barrier->lock);
        while(barrier->flag != *local);
    }
    
   /*
   if (__sync_fetch_and_sub(&barrier->counter, 1) == 1){
        barrier->counter = barrier->total;
        barrier->flag = *local;
    } else {        
        while(barrier->flag != *local);
    }
    */

}
void spin_barrier_destroy(spin_barrier* barrier) {
    //pthread_spin_destroy(&barrier->lock);    
    barrier = NULL;
}