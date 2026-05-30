#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define REPEAT 100

double bench_seq(size_t n_bytes) {
    volatile char *arr = (volatile char *)malloc(n_bytes);
    if (!arr) return -1.0;
    memset((void*)arr, 1, n_bytes);
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < REPEAT; r++)
        for (size_t i = 0; i < n_bytes; i++)
            (void)arr[i];
    clock_gettime(CLOCK_MONOTONIC, &t1);
    free((void*)arr);
    double ns = (t1.tv_sec - t0.tv_sec)*1e9
              + (t1.tv_nsec - t0.tv_nsec);
    return ns / (REPEAT * (double)n_bytes);
}

double bench_rand(size_t n_bytes) {
    size_t n = n_bytes / sizeof(int);
    int *arr = (int *)malloc(n * sizeof(int));
    size_t *idx = (size_t *)malloc(n * sizeof(size_t));
    if (!arr || !idx) return -1.0;
    for (size_t i = 0; i < n; i++) { arr[i] = i; idx[i] = i; }
    for (size_t i = n-1; i > 0; i--) {
        size_t j = rand() % (i+1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    volatile long sum = 0;
    for (int r = 0; r < REPEAT; r++)
        for (size_t i = 0; i < n; i++)
            sum += arr[idx[i]];
    clock_gettime(CLOCK_MONOTONIC, &t1);
    free(arr); free(idx);
    double ns = (t1.tv_sec-t0.tv_sec)*1e9+(t1.tv_nsec-t0.tv_nsec);
    return ns/(REPEAT*(double)n);
}

int main(void) {
    size_t sizes[] = {
        4*1024, 8*1024, 16*1024, 32*1024,
        64*1024, 128*1024, 256*1024,
        512*1024, 1024*1024, 4*1024*1024,
        16*1024*1024, 64*1024*1024
    };
    int n = sizeof(sizes)/sizeof(sizes[0]);
    printf("%-12s %10s %10s\n", "Size(KB)", "SEQ ns/b", "RAND ns/elem");
    for (int i = 0; i < n; i++) {
        double seq = bench_seq(sizes[i]);
        double rnd = bench_rand(sizes[i]);
        printf("%-12zu %10.3f %10.3f\n", sizes[i]/1024, seq, rnd);
    }
    return 0;
}
