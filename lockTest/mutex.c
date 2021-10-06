#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;
static pthread_mutex_t spinlock;

void *test_func(void *arg) {
    for (int i = 0; i < 100000; i++) {
            pthread_mutex_lock(&spinlock);
            count++;
            pthread_mutex_unlock(&spinlock);
    }
    return NULL;
}

int main(int argc, const char* argv[]) {
    pthread_t id[100];
    pthread_mutex_init(&spinlock, 0);
    for (int i = 0; i < 100; ++i) {
        pthread_create(&id[i], NULL, test_func, NULL);
    }
    for (int i = 0; i < 100; ++i) {
        pthread_join(id[i], NULL);

    }
    printf("%d\n", count);
    return 0;
}