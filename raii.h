#ifndef RAII_H
#define RAII_H

#include <stdlib.h>
#include <stdio.h>

#if defined(__GNUC__) || defined(__clang__)
    #define RAII_SUPPORTED 1
    #define _cleanup_(func) __attribute__((__cleanup__(func)))
#else
    #define RAII_SUPPORTED 0
    #define _cleanup_(func)
#endif

static inline void cleanup_free(void *p)
{
    free(*(void **)p);
}

static inline void cleanup_fclose(FILE **fp)
{
    if (*fp) {
        fclose(*fp);
        *fp = NULL;
    }
}

#ifdef _WIN32
#include <windows.h>

static inline void cleanup_close_handle(HANDLE *h)
{
    if (*h && *h != INVALID_HANDLE_VALUE) {
        CloseHandle(*h);
        *h = INVALID_HANDLE_VALUE;
    }
}

#define auto_handle _cleanup_(cleanup_close_handle)

#else

#include <unistd.h>
static inline void cleanup_close_fd(int *fd)
{
    if (*fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}

#define auto_fd _cleanup_(cleanup_close_fd)

#endif

#if RAII_SUPPORTED

#define auto_free   _cleanup_(cleanup_free)
#define auto_fclose _cleanup_(cleanup_fclose)

#define TAKE_PTR(ptr) \
    ({                \
        __typeof__(ptr) _tmp_ = (ptr); \
        (ptr) = NULL;                  \
        _tmp_;                         \
    })

#else

#define auto_free
#define auto_fclose
#define TAKE_PTR(ptr) (ptr)

#endif

#endif
