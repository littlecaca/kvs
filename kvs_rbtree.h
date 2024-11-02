#pragma once

typedef char *KEY_TYPE;
typedef char *VALUE_TYPE;

typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	VALUE_TYPE value;
} rbtree_node;

typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;

typedef void (*node_handler_cb)(rbtree_node *);

rbtree_node *rbtree_node_create(rbtree *T, KEY_TYPE key, VALUE_TYPE value);
rbtree *rbtree_create();
void rbtree_destroy(rbtree *T);
rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x);
rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x);
rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x);
void rbtree_insert(rbtree *T, rbtree_node *z);
void rbtree_delete(rbtree *T, rbtree_node *z);
rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key);
void rbtree_traversal(rbtree *T, rbtree_node *node, node_handler_cb handler);
