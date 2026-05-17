#include "raii.h"
#include <string.h>

int process_file(const char *filename)
{
    auto_fclose FILE *fp = fopen(filename, "w");

    if (!fp) {
        fprintf(stderr, "Cannot open %s for writing\n", filename);
        return -1;
    }

    auto_free char *msg = malloc(256);
    if (!msg)
        return -1;

    snprintf(msg, 256, "Written by RAII demo at compile time.\n");
    fputs(msg, fp);

    printf("  Wrote to %s successfully.\n", filename);
    return 0;
}

char *create_greeting(const char *name)
{
    auto_free char *buf = malloc(128);
    if (!buf) return NULL;

    snprintf(buf, 128, "Hello, %s! Welcome to RAII in C.", name);

    return TAKE_PTR(buf);
}

int multi_resource_example(void)
{
    printf("\n=== Multiple resources ===\n");

    auto_free char *buf1 = malloc(64);
    auto_free char *buf2 = malloc(64);
    auto_free char *buf3 = malloc(64);

    if (!buf1 || !buf2 || !buf3) {
        printf("  Allocation failed (all freed automatically)\n");
        return -1;
    }

    strcpy(buf1, "Resource 1");
    strcpy(buf2, "Resource 2");
    strcpy(buf3, "Resource 3");

    printf("  %s, %s, %s\n", buf1, buf2, buf3);
    printf("  All cleaned up on scope exit.\n");
    return 0;
}

int main(void)
{
    printf("=== raii.h Demo ===\n\n");
    printf("RAII_SUPPORTED = %d\n", RAII_SUPPORTED);

    printf("\n=== File + Memory auto-cleanup ===\n");
    process_file("raii_test_output.txt");

    printf("\n=== Ownership Transfer (TAKE_PTR) ===\n");
    char *greeting = create_greeting("Developer");
    if (greeting) {
        printf("  %s\n", greeting);
        free(greeting);
    }

    multi_resource_example();

    printf("\nAll demos passed.\n");
    return 0;
}
