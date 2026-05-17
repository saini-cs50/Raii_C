#include <stdlib.h>
#include <string.h>

static inline void cleanup_free(void *p)
{
    free(*(void **)p);
}

#define auto_free __attribute__((cleanup(cleanup_free)))

__attribute__((noinline))
int func_manual(void)
{
    char *buf = malloc(256);
    if (!buf)
        return -1;

    memset(buf, 0, 256);
    buf[0] = 'H';
    buf[1] = 'i';

    free(buf);
    return 0;
}

__attribute__((noinline))
int func_cleanup(void)
{
    auto_free char *buf = malloc(256);
    if (!buf)
        return -1;

    memset(buf, 0, 256);
    buf[0] = 'H';
    buf[1] = 'i';

    return 0;
}

int main(void)
{
    func_manual();
    func_cleanup();
    return 0;
}
