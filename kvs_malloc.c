#include "kvs_malloc.h"

#include <stdlib.h>

#include "logger.h"

static void kvs_oom_handler()
{
    LOG_ERROR("No enough memory to allocate.");
    abort();
}

void *kmalloc(size_t length)
{
    void *ret = malloc(length);
    if (ret == NULL)
    {
        kvs_oom_handler();
    }
    return ret;
}

void *ktrymalloc(size_t length)
{
    void *ret = malloc(length);
    return ret;
}
