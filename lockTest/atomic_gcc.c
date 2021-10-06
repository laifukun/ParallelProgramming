#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;

void *test_func(void *arg) {
    for (int i = 0; i < 100000; i++) {
        __sync_fetch_and_add(&count, 1);
    }
    return NULL;
}

int main(int argc, const char* argv[]) {
    pthread_t id[100];
    for (int i = 0; i < 100; ++i) {
        pthread_create(&id[i], NULL, test_func, NULL);
    }
    for (int i = 0; i < 100; ++i) {
        pthread_join(id[i], NULL);

    }
    printf("%d\n", count);
    return 0;
}