#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>

#include "kvs_hash.h"
#include "kvs_malloc.h"

#define FNV_PRIME 1099511628211ull

static inline uint64_t fnv1a_hash(const char *str) {
    uint64_t hash = 14695981039346656037ull;
    
    while (*str != 0) {
        hash ^= (uint8_t)*str;
        hash *= FNV_PRIME;
        ++str;
    }

    return hash;
}

//Connection 
// 'C' + 'o' + 'n'
static int _hash(const char *key, int size) {
	if (!key) return -1;
	return fnv1a_hash(key) % size;
}

static kvs_hash_node *_create_node(const char *key, const char *value) {

	kvs_hash_node *node = (kvs_hash_node *)kmalloc(sizeof(kvs_hash_node));
	char *kcopy = kmalloc(strlen(key) + 1);
	memset(kcopy, 0, strlen(key) + 1);
	strncpy(kcopy, key, strlen(key));

	node->key = kcopy;

	char *kvalue = kmalloc(strlen(value) + 1);
	memset(kvalue, 0, strlen(value) + 1);
	strncpy(kvalue, value, strlen(value));

	node->value = kvalue;
	
	node->next = NULL;
	return node;
}

static void _destroy_node(kvs_hash_node *node)
{
    assert(node != NULL);
    if (node->key != NULL) kfree(node->key);
    if (node->value != NULL) kfree(node->value);
    kfree(node);
}

kvs_hash *kvs_hash_create() {
    kvs_hash *hash_table = (kvs_hash *)kmalloc(sizeof(kvs_hash));
    hash_table->nodes = (kvs_hash_node **)kcalloc(1, sizeof(kvs_hash_node *) * MIN_TABLE_LENGTH);

	hash_table->buckets = MIN_TABLE_LENGTH;
	hash_table->count = 0; 
    return hash_table;
}

void kvs_hash_destroy(kvs_hash *hash) {
	if (!hash) return;

    int capacity = hash->buckets;
	for (int i = 0; i < capacity; ++i) {
		kvs_hash_node *node = hash->nodes[i];

		while (node != NULL) { // error
			kvs_hash_node *tmp = node;
			node = node->next;
			_destroy_node(tmp);
		}
        hash->nodes[i] = NULL;
	}

    kfree(hash->nodes);
    kfree(hash);
}

// 5 + 2

static void _extend_table(kvs_hash *hash)
{
    int capacity = hash->buckets * 2;
    if (capacity < hash->buckets)
    {
        fprintf(stderr, "Hash table size overflows.");
        abort();
    }
    kvs_hash_node **hash_table = (kvs_hash_node **)kcalloc(1, sizeof(kvs_hash_node *) * capacity);
    // rehash
    int count = 0;
    int old_capacity = hash->buckets;
    for (int i = 0; i < old_capacity; ++i)
    {
        kvs_hash_node *node = hash->nodes[i];
        while (node != NULL)
        {
            kvs_hash_node *next = node->next;
            int idx = _hash(node->key, capacity);
            node->next = hash_table[idx];
            hash_table[idx] = node;

            node = next;
            ++count;
        }
    }
    assert(count == hash->count);

    kfree(hash->nodes);
    hash->nodes = hash_table;
    hash->buckets = capacity;
}

/**
 * @return
 * 0, OK.
 * -1, the key has been inserted; 
 * -2, invalid arguments; 
 */
int kvs_hash_insert(kvs_hash *hash, const char *key, const char *value) {
	if (!hash || !key || !value) return -2;

    // check if already have the key
	int idx = _hash(key, hash->buckets);
	kvs_hash_node *node = hash->nodes[idx];
	while (node != NULL) {
		if (strcmp(node->key, key) == 0) { // exist
			return -1;
		}
		node = node->next;
	}

    // try extending the hash table
    if (hash->buckets * HASH_LOAD_FACTOR < hash->count + 1)
    {
        _extend_table(hash);
    }

	kvs_hash_node *new_node = _create_node(key, value);
	new_node->next = hash->nodes[idx];
	hash->nodes[idx] = new_node;
	
	hash->count ++;
	return 0;
}


kvs_hash_node *kvs_hash_search(kvs_hash *hash, const char *key) {
	if (!hash || !key) return NULL;

	int idx = _hash(key, hash->buckets);

	kvs_hash_node *node = hash->nodes[idx];

	while (node != NULL) {
		if (strcmp(node->key, key) == 0) {
			return node;
		}
		node = node->next;
	}

	return NULL;
}

/** 
 * Modify the value of node with the key `key` to value.
 * @return 
 * 0, ok.
 * -1, invalid argument; 
 * -2, can not find the node;
 */
int kvs_hash_modify(kvs_hash *hash, const char *key, const char *value) {
	if (!hash || !key || !value) return -1;

	kvs_hash_node *node = kvs_hash_search(hash, key);
    if (node == NULL) return -2;

    kvs_hash_node *new_node = _create_node(key, value);

    char *tmp_key = node->key;
    char *tmp_value = node->value;
    node->key = new_node->key;
    node->value = new_node->value;
    new_node->key = tmp_key;
    new_node->value = tmp_value;
    _destroy_node(new_node);

	return 0;
}

int kvs_hash_count(kvs_hash *hash) {
	return hash->count;
}

void kvs_hash_delete(kvs_hash *hash, kvs_hash_node *node) {
	if (!hash || !node) return;

    int idx = _hash(node->key, hash->buckets);

    kvs_hash_node *cur = hash->nodes[idx];
    kvs_hash_node *last = NULL;
    while (cur != NULL && cur != node)
    {
        last = cur;
        cur = cur->next;
    }

    if (cur == node)
    {
        if (last == NULL)
            hash->nodes[idx] = cur->next;
        else
            last->next = cur->next;
        _destroy_node(cur);
        --hash->count;
    }
}

int kvs_hash_exist(kvs_hash *hash, const char *key) {

	if (hash == NULL || key == NULL) return 0;
    return kvs_hash_search(hash, key) != NULL;
}
