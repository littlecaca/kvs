#include "kvs_protocol.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "kvs_rbtree.h"

typedef enum _kvs_query_cmd_t
{
    KVS_START = 0,
    KVS_SET = KVS_START,
    KVS_GET,
    KVS_MOD,
    KVS_EXIST,
    KVS_DEL,
    KVS_END
} kvs_query_cmd_t;

const char *kvs_query_str[] = 
{
    "SET",
    "GET",
    "MOD",
    "EXIST",
    "DEL"
};


#define JOIN(a, b) a##b


/*
宏参数的展开遵循“惰性展开”的原则，只有在需要时才会展开:
1. 直接传递给 ## 或 # 操作符的宏参数不会被进一步展开。
2. 传递给另一个宏（如 JOIN）时，参数会先被展开，然后才会传递进去。
*/
#define node_t(type) JOIN(type, _node)

#define create(type) JOIN(type, _create)
#define destroy(type) JOIN(type, _destroy)
#define modify(type) JOIN(type, _modify)
#define delete(type) JOIN(type, _delete)
#define search(type) JOIN(type, _search)
#define traversal(type) JOIN(type, _traversal)
#define insert(type) JOIN(type, _insert)

#define kvs_node_t node_t(KVS_ENGINE)
#define kvs_engine_t KVS_ENGINE

#define kvs_create(...) create(KVS_ENGINE)(__VA_ARGS__)
#define kvs_destroy(...) destroy(KVS_ENGINE)(__VA_ARGS__)
#define kvs_modify(...) modify(KVS_ENGINE)(__VA_ARGS__)
#define kvs_delete(...) delete(KVS_ENGINE)(__VA_ARGS__)
#define kvs_search(...) search(KVS_ENGINE)(__VA_ARGS__)
#define kvs_traversal(...) traversal(KVS_ENGINE)(__VA_ARGS__)
#define kvs_insert(...) insert(KVS_ENGINE)(__VA_ARGS__)

static struct _kvs_engine_handle
{
    kvs_engine_t *engine;
    size_t cnt;
} handler;

void kvs_init_engine()
{
    memset(&handler, 0, sizeof handler);
    handler.engine = kvs_create();
}

/**
 * Split tokens in query by the ' ' char 
 * @return -1, too many tokens; >= 0, the tokens number.
 */
static int kvs_split_tokens(char *query, char **tokens)
{
    assert(query != NULL && tokens != NULL);

    int idx = 0;
    char *token = strtok(query, " ");
    while (token != NULL)
    {
        if (idx == TOKENS_MAX_NUM)
            return -1;
        tokens[idx++] = token;
        token = strtok(NULL, " ");
    }

    if (idx > 0)
    {
        for (char *it = tokens[idx - 1]; *it != 0; ++it)
        {
            if (*it == '\r' && *(it + 1) == '\n')
            {
                *it = 0;
                break;
            }
        }
    }
    return idx;
}

static kvs_query_cmd_t kvs_resolve_cmd(const char *cmd)
{
    for (int i = KVS_START; i != KVS_END; ++i)
    {
        if (0 == strcmp(kvs_query_str[i], cmd))
        {
            return i;
        }
    }
    return KVS_END;
}

/**
 * Deal with the received all kinds of query string and put the
 * response into response.
 * @param len the length of response buffer.
 * @return -1, invalid query; -2 not found; -3 unknown command;
 * -4, output or encoding error.
 */
int kvs_deal_request(char *query, char *response, size_t len, int *nwrite)
{
    assert(query != NULL && response != NULL);
    
    *nwrite = 0;
    char *tokens[TOKENS_MAX_NUM];

    int token_num = kvs_split_tokens(query, tokens);

    if (token_num < 0)
    {
        *nwrite = snprintf(response, len - 1, "too may tokens\r\n");
        return -1;
    }

#ifdef KVS_TEST
    printf("received tokens: ");
    for (int i = 0; i < token_num; ++i)
    {
        printf("%d.%s ", i + 1, tokens[i]);
    }
    printf("\n");
#endif

    if (token_num < 2)
    {
        *nwrite = snprintf(response, len - 1, "Missing Arguments\r\n");
        return -1;
    }
    
    kvs_query_cmd_t cmd = kvs_resolve_cmd(tokens[0]);
    
    switch (cmd)
    {
    case KVS_SET: {
        if (token_num < 3)
        {
            *nwrite = snprintf(response, len - 1, "Missing Arguments\r\n");
            return -1;
        }
        int ret = kvs_insert(handler.engine, tokens[1], tokens[2]);
        if (ret == -1)
            *nwrite = snprintf(response, len - 1, "Repeated Key\r\n");
        else
            *nwrite = snprintf(response, len - 1, "OK\r\n");

    }
        break;
    case KVS_GET: {
        kvs_node_t *node = kvs_search(handler.engine, tokens[1]);
        if (node == handler.engine->nil)
        {
            *nwrite = snprintf(response, len - 1, "Not Found\r\n");
            return -2;
        }
        *nwrite = snprintf(response, len - 1, "%s\r\n", node->value);
    }
        break;
    case KVS_MOD: {
        if (token_num < 3)
        {
            *nwrite = snprintf(response, len - 1, "missing arguments\r\n");
            return -1;
        }
        int ret = kvs_modify(handler.engine, tokens[1], tokens[2]);
        if (ret < 0)
        {
            if (ret == 2)
            {
                *nwrite = snprintf(response, len - 1, "Not Found\r\n");
                return -2;
            }
        }
        else
        {
            *nwrite = snprintf(response, len - 1, "OK\r\n");
        }
    }
        break;
    case KVS_EXIST: {
        kvs_node_t *node = kvs_search(handler.engine, tokens[1]);
        if (node == handler.engine->nil)
            *nwrite = snprintf(response, len - 1, "NO\r\n");
        else
            *nwrite = snprintf(response, len - 1, "YES\r\n");
    }
        break;
    case KVS_DEL: {
        kvs_node_t *node = kvs_search(handler.engine, tokens[1]);
        if (node == handler.engine->nil)
        {
            *nwrite = snprintf(response, len - 1, "Not Found\r\n");
            return -2;
        }
        kvs_delete(handler.engine, node);
        *nwrite = snprintf(response, len - 1, "OK\r\n");
    }
        break;
    default: {
        *nwrite = snprintf(response, len - 1, "Unknown Command\r\n");
        return -3;
    }
    }

    if (*nwrite < 0)
    {
        *nwrite = snprintf(response, len - 1, "Failed\r\n");
        return -4;
    }

    if (*nwrite > len - 1)
        *nwrite = len - 1;

    return 0;
}
