#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long id;
    long start;
    long end;    // inclusive
    unsigned long long partial;
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    unsigned long long s = 0;
    for (long i = t->start; i <= t->end; i++) {
        s += (unsigned long long)i;
    }
    t->partial = s;
    printf("Thread %ld: range [%ld, %ld], partial = %llu\n",
           t->id, t->start, t->end, t->partial);
    return NULL;
}

int main(int argc, char** argv) {
    long N = (argc >= 2) ? strtol(argv[1], NULL, 10) : 4;
    long RANGE_START = 1;
    long RANGE_END   = 1000; // total range to sum; change as desired
    if (N <= 0) { fprintf(stderr, "N must be > 0\n"); return 1; }

    pthread_t* threads = malloc(sizeof(pthread_t) * N);
    Task* tasks = malloc(sizeof(Task) * N);

    long totalCount = RANGE_END - RANGE_START + 1;
    long base = totalCount / N;
    long rem  = totalCount % N;

    long cur = RANGE_START;
    for (long i = 0; i < N; i++) {
        long chunk = base + (i < rem ? 1 : 0);
        tasks[i].id = i;
        tasks[i].start = cur;
        tasks[i].end = cur + chunk - 1;
        tasks[i].partial = 0;
        cur += chunk;
        pthread_create(&threads[i], NULL, worker, &tasks[i]);
    }

    unsigned long long total = 0;
    for (long i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        total += tasks[i].partial;
    }

    printf("Total sum = %llu\n", total);

    free(threads);
    free(tasks);
    return 0;
}
