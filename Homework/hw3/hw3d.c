#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static long long counter = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static inline void counter_inc(void) {
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
}

typedef struct {
    long iters;
} Args;

void* worker(void* arg) {
    Args* a = (Args*)arg;
    for (long i = 0; i < a->iters; i++) {
        counter_inc();
    }
    return NULL;
}

int main(int argc, char** argv) {
    long T = (argc > 1) ? strtol(argv[1], NULL, 10) : 8;     // threads
    long N = (argc > 2) ? strtol(argv[2], NULL, 10) : 10000; // iters per thread
    if (T <= 0 || N < 0) { fprintf(stderr, "Usage: %s [threads] [iters]\n", argv[0]); return 1; }

    pthread_t* th = malloc(sizeof(pthread_t) * T);
    Args a = { .iters = N };

    for (long i = 0; i < T; i++) {
        if (pthread_create(&th[i], NULL, worker, &a) != 0) { perror("pthread_create"); return 1; }
    }
    for (long i = 0; i < T; i++) {
        pthread_join(th[i], NULL);
    }

    long long expected = (long long)T * N;
    printf("Expected = %lld, Observed counter = %lld\n", expected, counter);

    free(th);
    return 0;
}
