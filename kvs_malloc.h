#pragma once

#include <stdlib.h>

void *kmalloc(size_t length);
void *ktrymalloc(size_t length);
void *kcalloc(size_t count, size_t length);
void *ktrycalloc(size_t count, size_t length);
void *krealloc(void *ptr, size_t length);
void *ktryrealloc(void *ptr, size_t length);
void kfree(void *pointer);
