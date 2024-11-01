#pragma once

#include <stdlib.h>

// allocate memory or abort if out of memory
void *kmalloc(size_t length);

// try allocating memory or return NULL if out of memory
void *ktrymalloc(size_t length);
