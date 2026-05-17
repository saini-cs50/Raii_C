#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS    5000000
#define ALLOC_SIZE    256
#define NUM_RESOURCES 3
#define WARMUP_ITERS  500000

static void use_ptr(void *p)
{
    __asm__ volatile("" : : "r"(p) : "memory");
}

static inline void cleanup_free(void *p)
{
    free(*(void **)p);
}

#define auto_free __attribute__((cleanup(cleanup_free)))

static double time_diff_ms(struct timespec start, struct timespec end)
{
    double s  = (double)(end.tv_sec  - start.tv_sec)  * 1000.0;
    double ns = (double)(end.tv_nsec - start.tv_nsec) / 1000000.0;
    return s + ns;
}

__attribute__((noinline))
void bench_manual(void)
{
    char *a = malloc(ALLOC_SIZE);
    char *b = malloc(ALLOC_SIZE);
    char *c = malloc(ALLOC_SIZE);

    if (!a || !b || !c) goto fail;

    memset(a, 'A', ALLOC_SIZE);
    memset(b, 'B', ALLOC_SIZE);
    memset(c, 'C', ALLOC_SIZE);
    use_ptr(a);
    use_ptr(b);
    use_ptr(c);

fail:
    free(c);
    free(b);
    free(a);
}

__attribute__((noinline))
void bench_cleanup(void)
{
    auto_free char *a = malloc(ALLOC_SIZE);
    auto_free char *b = malloc(ALLOC_SIZE);
    auto_free char *c = malloc(ALLOC_SIZE);

    if (!a || !b || !c) return;

    memset(a, 'A', ALLOC_SIZE);
    memset(b, 'B', ALLOC_SIZE);
    memset(c, 'C', ALLOC_SIZE);
    use_ptr(a);
    use_ptr(b);
    use_ptr(c);
}

__attribute__((noinline))
void bench_goto(void)
{
    char *a = NULL, *b = NULL, *c = NULL;

    a = malloc(ALLOC_SIZE);
    if (!a) goto cleanup;

    b = malloc(ALLOC_SIZE);
    if (!b) goto cleanup;

    c = malloc(ALLOC_SIZE);
    if (!c) goto cleanup;

    memset(a, 'A', ALLOC_SIZE);
    memset(b, 'B', ALLOC_SIZE);
    memset(c, 'C', ALLOC_SIZE);
    use_ptr(a);
    use_ptr(b);
    use_ptr(c);

cleanup:
    free(c);
    free(b);
    free(a);
}

typedef void (*bench_fn)(void);

double run_benchmark(const char *name, bench_fn fn, long iters)
{
    struct timespec start, end;

    for (long i = 0; i < WARMUP_ITERS; i++)
        fn();

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < iters; i++)
        fn();
    clock_gettime(CLOCK_MONOTONIC, &end);

    double ms = time_diff_ms(start, end);
    double per_iter_ns = (ms * 1000000.0) / (double)iters;

    printf("  %-25s %10.2f ms total   %8.1f ns/iter\n",
           name, ms, per_iter_ns);

    return ms;
}

int main(void)
{
    printf("RAII Benchmark: cleanup vs manual vs goto\n\n");
    printf("Config: %d iterations, %d bytes x %d resources\n",
           ITERATIONS, ALLOC_SIZE, NUM_RESOURCES);
    printf("Compiler: GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

#ifdef __OPTIMIZE__
    printf("Optimization: ENABLED (-O2)\n\n");
#else
    printf("Optimization: DISABLED (-O0)\n\n");
#endif

    double t_manual  = run_benchmark("Manual free()",      bench_manual,  ITERATIONS);
    double t_cleanup = run_benchmark("cleanup attribute",   bench_cleanup, ITERATIONS);
    double t_goto    = run_benchmark("goto cleanup",        bench_goto,    ITERATIONS);

    printf("\n--- Relative Performance ---\n");
    printf("  cleanup vs manual:  %+.2f%%\n",
           ((t_cleanup - t_manual) / t_manual) * 100.0);
    printf("  cleanup vs goto:    %+.2f%%\n",
           ((t_cleanup - t_goto) / t_goto) * 100.0);
    printf("  goto vs manual:     %+.2f%%\n",
           ((t_goto - t_manual) / t_manual) * 100.0);

    return 0;
}
