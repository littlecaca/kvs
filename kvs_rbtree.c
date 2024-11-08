#include "kvs_rbtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kvs_malloc.h"

#define RED				1
#define BLACK 			2

/* Optional settings */
static inline int key_cmp(KEY_TYPE left, KEY_TYPE right)
{
    return strcmp(left, right);
}

/* Optional settings */
static inline void node_kv_free(rbtree_node *pnode)
{
    if (!pnode->key) kfree((void *)pnode->key);
    if (!pnode->value) kfree((void *)pnode->value);
    pnode->key = NULL;
    pnode->value = NULL;
}

/* Optional settings */
static inline KEY_TYPE key_cpy(KEY_TYPE key)
{
    size_t len = strlen(key);
    char *new_key = kmalloc(sizeof(char) * len + 1);
    strncpy(new_key, key, len + 1);
    return new_key;
}

/* Optional settings */
static inline VALUE_TYPE value_cpy(VALUE_TYPE value)
{
    size_t len = strlen(value);
    char *new_key = kmalloc(sizeof(char) * len + 1);
    strncpy(new_key, value, len + 1);
    return new_key;
}

/* Optional settings */
void value_replace(rbtree_node *node,  VALUE_TYPE value)
{
    assert(value != NULL);
    if (node->value != value)
    {
        if (node->value) kfree((void *)node->value);
        node->value = value_cpy(value);
    }
}

/** 
 * Modify the value of node with the key `key` to value.
 * @return -1, invalid argument; -2, can not find the node;
 * 0, ok.
 */
int rbtree_modify(rbtree *T, KEY_TYPE key, VALUE_TYPE value)
{
    if (value == NULL || key == NULL)
        return -1;
    rbtree_node *node = rbtree_search(T, key);
    if (node == NULL)
        return -2;
    value_replace(node, value);
    return 0;
}

int rbtree_exist(rbtree *T, KEY_TYPE key)
{
	if (T == NULL || key == NULL)
		return 0;
	return rbtree_search(T, key) != NULL;
}

rbtree_node *rbtree_node_create(rbtree *T, KEY_TYPE key, VALUE_TYPE value)
{
    rbtree_node *node = kmalloc(sizeof(rbtree_node));
    node->color = RED;
    node->key = key_cpy(key);
    node->value = value_cpy(value);
    node->left = T->nil;
    node->right = T->nil;
    return node;
}

rbtree *rbtree_create()
{
    rbtree *T = (rbtree *)kmalloc(sizeof(rbtree));
    T->nil = kmalloc(sizeof(rbtree_node));
    T->nil->color = BLACK;
    T->root = T->nil;
    return T;
}

static void _rbtree_destory_internal(rbtree *T, rbtree_node *cur)
{
    if (cur == T->nil) return;
    _rbtree_destory_internal(T, cur->left);
    _rbtree_destory_internal(T, cur->right);
    node_kv_free(cur);
    kfree(cur);
}

void rbtree_destroy(rbtree *T)
{
    _rbtree_destory_internal(T, T->root);
    kfree(T->nil);
    kfree(T);
}

rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x) {
	while (x->left != T->nil) {
		x = x->left;
	}
	return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x) {
	while (x->right != T->nil) {
		x = x->right;
	}
	return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x) {
	rbtree_node *y = x->parent;

	if (x->right != T->nil) {
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right)) {
		x = y;
		y = y->parent;
	}
	return y;
}


static void rbtree_left_rotate(rbtree *T, rbtree_node *x) {

	rbtree_node *y = x->right;  // x  --> y  ,  y --> x,   right --> left,  left --> right

	x->right = y->left; //1 1
	if (y->left != T->nil) { //1 2
		y->left->parent = x;
	}

	y->parent = x->parent; //1 3
	if (x->parent == T->nil) { //1 4
		T->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x; //1 5
	x->parent = y; //1 6
}


static void rbtree_right_rotate(rbtree *T, rbtree_node *y) {

	rbtree_node *x = y->left;

	y->left = x->right;
	if (x->right != T->nil) {
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

static void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {

	while (z->parent->color == RED) { //z ---> RED
		if (z->parent == z->parent->parent->left) {
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {

				if (z == z->parent->right) {
					z = z->parent;
					rbtree_left_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}else {
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
		
	}

	T->root->color = BLACK;
}

/**
 * @return -1, the key has been inserted. 0, OK.
 */
int rbtree_insert(rbtree *T, KEY_TYPE key, VALUE_TYPE value) {
    rbtree_node *z = rbtree_node_create(T, key, value);

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil) {
		y = x;
        int ret = key_cmp(z->key, x->key);
		if (ret < 0) {
			x = x->left;
		} else if (ret > 0) {
			x = x->right;
		} else { //Exist
            kfree(z);
			return -1;
		}
	}

	z->parent = y;
	if (y == T->nil) {
		T->root = z;
	} else if (key_cmp(z->key, y->key) < 0) {
		y->left = z;
	} else {
		y->right = z;
	}

	rbtree_insert_fixup(T, z);
    return 0;
}

static void rbtree_delete_fixup(rbtree *T, rbtree_node *x) {

	while ((x != T->root) && (x->color == BLACK)) {
		if (x == x->parent->left) {

			rbtree_node *w= x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;

				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);

				x = T->root;
			}

		} else {

			rbtree_node *w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);

				x = T->root;
			}

		}
	}

	x->color = BLACK;
}

/**
 * @return -1, the key does not exist. 0, OK.
 */
int rbtree_delete(rbtree *T, KEY_TYPE key) 
{
	rbtree_node *z = rbtree_search(T, key);
	if (z == NULL) return -1;

	rbtree_node *y = T->nil;
    (void)y;
	rbtree_node *x = T->nil;

	if ((z->left == T->nil) || (z->right == T->nil)) {
		y = z;
	} else {
		y = rbtree_successor(T, z);
	}

	if (y->left != T->nil) {
		x = y->left;
	} else if (y->right != T->nil) {
		x = y->right;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->left) {
		y->parent->left = x;
	} else {
		y->parent->right = x;
	}

	if (y != z) {
        node_kv_free(z);
        
		z->key = y->key;
		z->value = y->value;
	}

	if (y->color == BLACK) {
		rbtree_delete_fixup(T, x);
	}

    kfree(y);
	
	return 0;
}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {

	rbtree_node *node = T->root;
	while (node != T->nil) {
        int ret = key_cmp(key, node->key);
		if (ret < 0) {
			node = node->left;
		} else if (ret > 0) {
			node = node->right;
		} else {
			return node;
		}	
	}
	return NULL;
}


void rbtree_traversal(rbtree *T, rbtree_node *node, node_handler_cb handler_cb) {
	if (node != T->nil) {
		rbtree_traversal(T, node->left, handler_cb);
		handler_cb(node);
		rbtree_traversal(T, node->right, handler_cb);
	}
}
