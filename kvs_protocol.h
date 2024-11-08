#pragma once

#include <stdlib.h>

#define TOKENS_MAX_NUM 32

#define KVS_RBTREE rbtree
#define KVS_HASH kvs_hash
#define KVS_SKIP_LIST skiplist

#define KVS_ENGINE KVS_SKIP_LIST


void kvs_init_engine();
void kvs_destroy_engine();

int kvs_deal_request(char *query, char *response, size_t len, int *nwrite);
