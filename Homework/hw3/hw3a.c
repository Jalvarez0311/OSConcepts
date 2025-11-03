#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static long long counter = 0;   

typedef struct {
    long id;
    long iters;
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    for (long i = 0; i < t->iters; i++) {
        counter++;  
    }
    return NULL;
}

int main(int argc, char** argv) {
    long N = (argc >= 2) ? strtol(argv[1], NULL, 10) : 4;
    long iters_per_thread = 10000;
    if (N <= 0) { fprintf(stderr, "N must be > 0\n"); return 1; }

    pthread_t* threads = malloc(sizeof(pthread_t) * N);
    Task* tasks = malloc(sizeof(Task) * N);

    for (long i = 0; i < N; i++) {
        tasks[i].id = i;
        tasks[i].iters = iters_per_thread;
        pthread_create(&threads[i], NULL, worker, &tasks[i]);
    }

    for (long i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Expected = %ld, Observed counter = %lld\n",
           N * iters_per_thread, counter);

    free(threads);
    free(tasks);
    return 0;
}
