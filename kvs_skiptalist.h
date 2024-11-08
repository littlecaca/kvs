typedef const char * KEY_TYPE;
typedef const char * VALUE_TYPE;

typedef struct _skiplist_node {
    KEY_TYPE key;
    VALUE_TYPE value;
    struct _skiplist_node **forwards;
} skiplist_node;

typedef struct _skiplist {
    int level;
    skiplist_node* header;
} skiplist;

skiplist* skiplist_create();
void skiplist_destroy(skiplist *skipList);
int skiplist_insert(skiplist* skipList, KEY_TYPE key, VALUE_TYPE value);
skiplist_node* skiplist_search(skiplist* skipList, KEY_TYPE key);
int skiplist_delete(skiplist *skipList, KEY_TYPE key);
int skiplist_exist(skiplist *skipList, KEY_TYPE key);
int skiplist_modify(skiplist *skipList, KEY_TYPE key, VALUE_TYPE value);
void display(skiplist* skipList);
