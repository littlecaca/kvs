#include "kvs_rbtree.h"

#include <stdio.h>

void handle_node(rbtree_node *node)
{
    printf("key: %s, value: %s\n", node->key, node->value);
}

int main() {
	// int keyArray[20] = {24,25,13,35,23, 26,67,47,38,98, 20,19,17,49,12, 21,9,18,14,15};
    char *keyArray[8] = {"niko", "king", "darren", "viic", "naci", "taozi", "youzi", "lulu"};
    char *valueArray[8] = {"1niko", "2king", "3darren", "4viic", "5naci", 
        "6taozi", "7youzi", "8lulu"};

	rbtree *T = rbtree_create();

	rbtree_node *node;
	int i = 0;
	for (i = 0;i < 8;i ++) {
		rbtree_insert(T, keyArray[i], valueArray[i]);
	}

	rbtree_traversal(T, T->root, &handle_node);
	printf("----------------------------------------\n");

	for (i = 0;i < 8;i ++) {
		rbtree_delete(T, keyArray[i]);

		rbtree_traversal(T, T->root, &handle_node);
		printf("----------------------------------------\n");
	}

    rbtree_destroy(T);
}
