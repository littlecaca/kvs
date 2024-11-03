#pragma once

#include <stdlib.h>

#define TOKENS_MAX_NUM 32

#define KVS_RBTREE rbtree
#define KVS_HASH hash
#define KVS_SKIP_LIST skip_list

#define KVS_ENGINE KVS_RBTREE


void kvs_init_engine();

int kvs_deal_request(char *query, char *response, size_t len, int *nwrite);
