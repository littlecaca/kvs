#include "kvs_malloc.h"

#include <stdlib.h>

#include "logger.h"

static void kvs_oom_handler()
{
    LOG_ERROR("No enough memory to allocate.");
    abort();
}

/* Allocate memory or abort if out of memory */
void *kmalloc(size_t length)
{
    void *ret = malloc(length);
    if (ret == NULL)
    {
        kvs_oom_handler();
    }
    return ret;
}

/**
 * Try allocating memory.
 * @return NUll, if out of memory; void *, if success;
 */
void *ktrymalloc(size_t length)
{
    void *ret = malloc(length);
    return ret;
}

void *kcalloc(size_t count, size_t length)
{
    void *ret = calloc(1, length);
    if (ret == NULL)
    {
        kvs_oom_handler();
    }
    return ret;
}

/**
 * Try allocating memory and set it all to 0.
 * @return NUll, if out of memory; void *, if success;
 */
void *ktrycalloc(size_t count, size_t length)
{
    void *ret = calloc(1, length);
    return ret;
}

/* Realloc the memory pointed by ptr with the new length. */
void *krealloc(void *ptr, size_t length)
{
    void *ret = realloc(ptr, length);
    if (ret == NULL)
    {
        kvs_oom_handler();
    }
    return ret;
}

/**
 * Realloc the memory pointed by ptr with the new length.
 * @return NUll, if out of memory; void *, if success;
 */
void *ktryrealloc(void *ptr, size_t length)
{
    void *ret = realloc(ptr, length);
    return ret;
}

void kfree(void *pointer)
{
    if (pointer == NULL)
    {
        LOG_ERROR("try to free a null ptr");
        return;
    }
    free(pointer);
}
