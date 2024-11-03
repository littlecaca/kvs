#include "kvs_socket.h"

#include "logger.h"

int kvs_tcp_socket(void)
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        LOG_SYSERR("socket fails");
        abort();
    }

    return fd;
}

void kvs_bind(int sock_fd, struct sockaddr *addr)
{
    if (-1 == bind(sock_fd, addr, sizeof addr))
    {
        LOG_SYSERR("bind() fails");
        abort();
    }
}

int kvs_send(int fd, const char *buffer, size_t len, int flags)
{
    int ret = send(fd, buffer, len, flags);
    if (-1 == ret)
    {
        LOG_SYSERR("send() fails");
        abort();
    }
    return ret;
}
