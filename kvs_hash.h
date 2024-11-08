#define MIN_TABLE_LENGTH 32
#define HASH_LOAD_FACTOR 0.65

typedef struct _kvs_hash_node {
	char *key;
	char *value;
	struct _kvs_hash_node *next;
} kvs_hash_node;

typedef struct _kvs_hash {
	kvs_hash_node **nodes; //* change **, 
	int buckets;
	int count;
} kvs_hash;

kvs_hash *kvs_hash_create();
void kvs_hash_destroy(kvs_hash *hash);
int kvs_hash_insert(kvs_hash *hash, const char *key, const char *value);
kvs_hash_node *kvs_hash_search(kvs_hash *hash, const char *key);
int kvs_hash_modify(kvs_hash *hash, const char *key, const char *value);
int kvs_hash_delete(kvs_hash *hash, const char *key);
int kvs_hash_exist(kvs_hash *hash, const char *key);
