#include "kvs_client.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "kvs_socket.h"
#include "logger.h"

int kvs_connect(const char *ip, unsigned short port)
{
    int sock_fd = kvs_tcp_socket();

    struct sockaddr_in server_addr;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int fail_cnt = 0;
    int timeout = MIN_TIMEOUT_MS;
    int kvs_errno;
    while (fail_cnt < MAX_REPEAT_TIMES)
    {
        if (-1 != connect(sock_fd, (struct sockaddr *)&server_addr, sizeof server_addr))
            break;
        kvs_errno = errno;
        usleep(timeout * 1000);
        timeout *= 2;
        ++fail_cnt;
    }

    if (fail_cnt == MAX_REPEAT_TIMES)
    {
        errno = kvs_errno;
        LOG_SYSERR("connecting timeout");
        return -1;
    }

    return sock_fd;
}

int kvs_close(int fd)
{
    close(fd);
    return 0;
}
