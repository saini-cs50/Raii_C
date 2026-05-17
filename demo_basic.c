#include <stdio.h>
#include <stdlib.h>

void cleanup_int(int *value)
{
    printf("  [cleanup] int at %p = %d\n", (void *)value, *value);
}

void cleanup_free(void *p)
{
    void *ptr = *(void **)p;
    printf("  [cleanup] freeing pointer %p\n", ptr);
    free(ptr);
}

void example_scope(void)
{
    printf("\n=== Example 1: Scope-based cleanup ===\n");

    __attribute__((cleanup(cleanup_int)))
    int x = 42;

    __attribute__((cleanup(cleanup_int)))
    int y = 99;

    printf("  x = %d, y = %d\n", x, y);
    printf("  Leaving scope...\n");
}

void example_memory(void)
{
    printf("\n=== Example 2: Automatic memory cleanup ===\n");

    __attribute__((cleanup(cleanup_free)))
    char *buffer = malloc(256);

    if (!buffer) {
        printf("  malloc failed\n");
        return;
    }

    snprintf(buffer, 256, "Hello from auto-managed memory!");
    printf("  buffer = \"%s\"\n", buffer);
    printf("  Leaving scope (buffer freed automatically)...\n");
}

void example_early_return(int should_return_early)
{
    printf("\n=== Example 3: Early return (early=%d) ===\n",
           should_return_early);

    __attribute__((cleanup(cleanup_free)))
    char *data = malloc(128);

    if (!data) return;

    snprintf(data, 128, "Some allocated data");
    printf("  data = \"%s\"\n", data);

    if (should_return_early) {
        printf("  Returning early...\n");
        return;
    }

    printf("  Reached end of function.\n");
}

void example_lifo_order(void)
{
    printf("\n=== Example 4: LIFO cleanup order ===\n");

    __attribute__((cleanup(cleanup_int)))
    int a = 1;

    __attribute__((cleanup(cleanup_int)))
    int b = 2;

    __attribute__((cleanup(cleanup_int)))
    int c = 3;

    printf("  Declared a=%d, b=%d, c=%d\n", a, b, c);
    printf("  Leaving scope (expect: c, b, a)...\n");
}

int main(void)
{
    printf("RAII in C: Demonstration of __attribute__((cleanup))\n");
    printf("Compiler: GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

    example_scope();
    example_memory();
    example_early_return(0);
    example_early_return(1);
    example_lifo_order();

    printf("\nAll examples complete.\n");
    return 0;
}
