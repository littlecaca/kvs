#include "server.h"

#include <string.h>
#include <stdlib.h>

int kvs_request(connection *conn)
{
    printf("read from client %d: %s\n", conn->fd, conn->rbuffer);
    
    memcpy(conn->wbuffer, conn->rbuffer, conn->rlength);
    conn->wlength = conn->rlength;

    return 0;
}

int kvs_close(connection *conn)
{
    printf("close client %d\n", conn->fd);
    return 0;
}

int kvs_accept(connection *conn)
{
    printf("accept new client %d\n", conn->fd);
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("usage: kvstore <port>\n");
        return 1;
    }

    int port = atoi(argv[1]);

    EventHandler handler = {0};
    handler.request_handler = &kvs_request;
    handler.close_handler = &kvs_close;
    handler.accept_handler = &kvs_accept;

    server_start(port, &handler);
    return 0;
}
