#pragma once

#define MAX_REPEAT_TIMES 3
#define MIN_TIMEOUT_MS 50

int kvs_connect(const char *ip, unsigned short port);

int kvs_close(int fd);
