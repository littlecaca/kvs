#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

int kvs_tcp_socket(void);

void kvs_bind(int fd, struct sockaddr *addr);

int kvs_send(int fd, const char *buffer, size_t len, int flags);

// int kvs_recv(int fd, const char *buffer, size_t len);

// int kvs_set_noblock(int fd);

// int kvs_set_nodelay(int fd);

// int kvs_close(int fd);
