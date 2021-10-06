#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;
static pthread_spinlock_t spinlock;

void *test_func(void *arg) {
    for (int i = 0; i < 100000; i++) {
        //printf("Enter");
            pthread_spin_lock(&spinlock);
            count++;
            pthread_spin_unlock(&spinlock);
        //printf("Exit");
    }
    return NULL;
}

int main(int argc, const char* argv[]) {
    pthread_t id[100];
    pthread_spin_init(&spinlock, 0);
    for (int i = 0; i < 100; ++i) {
        pthread_create(&id[i], NULL, test_func, NULL);
    }

    for (int i = 0; i < 100; ++i) {
        pthread_join(id[i], NULL);

    }
    printf("%d\n", count);
    return 0;
}