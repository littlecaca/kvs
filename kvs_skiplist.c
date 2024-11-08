#include "kvs_skiptalist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kvs_malloc.h"

#define KVS_MAX_LEVEL 16
#define KVS_SKIPLIST_P 0.5

static char *strCopy(const char *src)
{
    if (src == NULL) return NULL;
    size_t len = strlen(src);
    char *res = kmalloc(len + 1);
    strncpy(res, src, len + 1);
    return res;
}

static skiplist_node* createNode(int level, KEY_TYPE key, VALUE_TYPE value) {
    skiplist_node* newNode = (skiplist_node*)kmalloc(sizeof(skiplist_node));
    newNode->key = strCopy(key);
    newNode->value = strCopy(value);
    newNode->forwards = (skiplist_node**)kcalloc(1, level * sizeof(skiplist_node*));

    return newNode;
}

static void destroyNode(skiplist_node *node)
{
    assert(node != NULL);
    kfree(node->forwards);
    kfree((void *)node->value);
    kfree((void *)node->key);
    kfree(node);
}

static int randomLevel() {
   int level = 1;
   while ((rand() & 0xFFFF) < (0xFFFF * KVS_SKIPLIST_P))
      level++;
   return (level < KVS_MAX_LEVEL) ? level : KVS_MAX_LEVEL;
}

skiplist* skiplist_create() {
    skiplist* skipList = (skiplist*)kmalloc(sizeof(skiplist));
    skipList->level = 1;
    skipList->header = createNode(KVS_MAX_LEVEL, NULL, NULL);
    
    for (int i = 0; i < KVS_MAX_LEVEL; ++i) {
        skipList->header->forwards[i] = NULL;
    }
    
   return skipList; 
}

/**
 * @return -1, the key has been inserted. 0, OK.
 */
int skiplist_insert(skiplist* skipList, KEY_TYPE key, VALUE_TYPE value) {
    skiplist_node* update[KVS_MAX_LEVEL];
    skiplist_node* current = skipList->header;
    
    assert(skipList->level > 0);
    for (int i = skipList->level - 1; i >= 0; --i) {
        while (current->forwards[i] != NULL && strcmp(current->forwards[i]->key, key) < 0)
            current = current->forwards[i];
        update[i] = current;
    }

    current = current->forwards[0];

    if (current == NULL || strcmp(current->key, key) != 0) 
    {
            int level = randomLevel();
            // Adjust the level of skiplist
            if (level > skipList->level) {
                for (int i = skipList->level; i < level; ++i)
                    update[i] = skipList->header;
                skipList->level = level;
            }

            skiplist_node* newNode = createNode(level, key, value);
            // Insert new node
            for (int i = 0; i < level; ++i) {
                newNode->forwards[i] = update[i]->forwards[i];
                update[i]->forwards[i] = newNode;
            }
            
            return 0;
    } 
    // The key already exists
    return -1;
}

void display(skiplist* skipList) {
    printf("Skip List:\n");
    
    for (int i = 0; i <= skipList->level; ++i) {
        skiplist_node* node = skipList->header->forwards[i];
        printf("Level %d: ", i);
        
        while (node != NULL) {
            printf("%s ", node->key);
            node = node->forwards[i];
        }
        
        printf("\n");
    }
}

skiplist_node* skiplist_search(skiplist* skipList, KEY_TYPE key) {
    skiplist_node* current = skipList->header;

    for (int i = skipList->level - 1; i >= 0; --i) 
    {
        while (current->forwards[i] != NULL && strcmp(current->forwards[i]->key, key) < 0)
            current = current->forwards[i];
    }

    current = current->forwards[0];

    if(current && strcmp(current->key, key) == 0)
        return current;
    else
        return NULL;
}

/**
 * @return -1, the key does not exist. 0, OK.
 */
int skiplist_delete(skiplist *skipList, KEY_TYPE key)
{
    skiplist_node *update[KVS_MAX_LEVEL];
    skiplist_node *current = skipList->header;

    for (int i = skipList->level - 1; i >= 0; --i)
    {
        while (current->forwards[i] != NULL && strcmp(current->forwards[i]->key, key) < 0)
            current = current->forwards[i];
        update[i] = current;
    }

    if (current->forwards[0] != NULL && strcmp(current->forwards[0]->key, key) == 0)
    {
        skiplist_node *tar = current->forwards[0];
        for (int i = 0; i < skipList->level; ++i)
        {
            if (update[i]->forwards[i] == tar)
            {
                update[i]->forwards[i] = tar->forwards[i];
            }
        }

        destroyNode(tar);

        while (skipList->level > 1 && skipList->header->forwards[skipList->level - 1] == NULL)
            --skipList->level;
        return 0;
    }

    return -1;
}


int skiplist_exist(skiplist *skipList, KEY_TYPE key)
{
    if (skipList == NULL || key == NULL)
        return 0;
    return skiplist_search(skipList, key) != NULL;
}

/** 
 * Modify the value of node with the key `key` to value.
 * @return -1, invalid argument; -2, can not find the node;
 * 0, ok.
 */
int skiplist_modify(skiplist *skipList, KEY_TYPE key, VALUE_TYPE value)
{
    if (skipList == NULL || key == NULL || value == NULL)
        return -1;
    
    skiplist_node *tar = skiplist_search(skipList, key);

    if (tar == NULL) return -2;

    kfree((void *)tar->value);
    tar->value = strCopy(value);
    return 0;
}


void skiplist_destroy(skiplist *skipList) 
{
    if (skipList == NULL) return;

    for (skiplist_node *current = skipList->header; current != NULL;)
    {
        skiplist_node *cur = current;
        current = current->forwards[0];
        destroyNode(cur);
    }

    kfree(skipList);
}

