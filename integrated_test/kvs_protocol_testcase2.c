#include "logger.h"
#include "kvs_client.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>

#include "kvs_socket.h"

void testcase(int insert_cnt, int query_cnt)
{
    assert(insert_cnt > 0 && query_cnt > 0);

    int fd = kvs_connect("127.0.0.1", 2000);
    char write_buffer[1024];
    char read_buffer[1024];

    struct timeval begin = {0};
    struct timeval end = {0};
    gettimeofday(&begin, NULL);

    for (int k = 0; k < insert_cnt; ++k)
    {
        int len = snprintf(write_buffer, sizeof write_buffer,
             "SET teacher%d %s\r\n", k, "mind");
        kvs_send(fd, write_buffer, len, 0);
        
        int ret = recv(fd, read_buffer, sizeof read_buffer - 1, 0);
        if (ret == -1)
        {
            LOG_SYSERR("recv() fails");
            abort();
        }

        read_buffer[ret] = 0;
        if (strcmp(read_buffer, "OK\r\n") != 0)
        {
            printf("error: ---> %s", write_buffer);
            printf("<---------- %s", read_buffer);
            printf("expected: %s","OK\r\n");
        }
    }

    for (int k = 0; k < query_cnt; ++k)
    {
        int len = snprintf(write_buffer, sizeof write_buffer,
             "GET teacher%d\r\n", k);
        kvs_send(fd, write_buffer, len, 0);
        
        int ret = recv(fd, read_buffer, sizeof read_buffer - 1, 0);
        if (ret == -1)
        {
            LOG_SYSERR("recv() fails");
            abort();
        }

        read_buffer[ret] = 0;
        if (strcmp(read_buffer, "mind\r\n") != 0)
        {
            printf("error: ---> %s", write_buffer);
            printf("<---------- %s", read_buffer);
            printf("expected: %s","mind\r\n");
        }
    }

    for (int k = 0; k < insert_cnt; ++k)
    {
        int len = snprintf(write_buffer, sizeof write_buffer,
             "DEL teacher%d\r\n", k);
        kvs_send(fd, write_buffer, len, 0);
        
        int ret = recv(fd, read_buffer, sizeof read_buffer - 1, 0);
        if (ret == -1)
        {
            LOG_SYSERR("recv() fails");
            abort();
        }

        read_buffer[ret] = 0;
        if (strcmp(read_buffer, "OK\r\n") != 0)
        {
            printf("error: ---> %s", write_buffer);
            printf("<---------- %s", read_buffer);
            printf("expected: %s","OK\r\n");
        }
    }

    gettimeofday(&end, NULL);


    int request_cnt = insert_cnt * 2 + query_cnt;
    uint64_t total_time = (end.tv_sec - begin.tv_sec) * 1000 
        + (end.tv_usec - begin.tv_usec) / 1000;
    int qps = 0;
    if (total_time != 0)
    {
        qps = request_cnt / total_time * 1000;
    }
    
    printf("=========================\n");
    printf("request_cnt: %d, total_time(ms): %ld, qps: %d\n", request_cnt,
        total_time, qps);

    kvs_close(fd);
 }

 int main(int argc, char const *argv[])
 {
    testcase(10000, 10000);
    return 0;
 }
 