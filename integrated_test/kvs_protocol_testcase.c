#include "logger.h"
#include "kvs_client.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <stdint.h>

#include "kvs_socket.h"

int main(int argc, char const *argv[])
{
    // 13 commands
    const char *input[] = {
        "SET teacher1 darren\r\n",
        "SET teacher2 king\r\n",
        "SET teacher3 taozi\r\n",

        "GET teacher1\r\n",
        "GET teacher2\r\n",
        "GET teacher3\r\n",

        "MOD teacher1 niko\r\n",
        "GET teacher1\r\n",

        "DEL teacher1\r\n",
        "DEL teacher2\r\n",
        "EXIST teacher1\r\n",
        "EXIST teacher3\r\n",
        "DEL teacher3\r\n"
    };

    const char *output[] = {
        "OK\r\n",
        "OK\r\n",
        "OK\r\n",

        "darren\r\n",
        "king\r\n",
        "taozi\r\n",

        "OK\r\n",
        "niko\r\n",

        "OK\r\n",
        "OK\r\n",
        "NO\r\n",
        "YES\r\n",
        "OK\r\n"
    };

    const int cmd_cnt = 13;

    int fd = kvs_connect("127.0.0.1", 2000);
    char buffer[1024];

    int request_cnt = 0;
    int polls = 1000;

    struct timeval begin = {0};
    struct timeval end = {0};
    gettimeofday(&begin, NULL);

    for (int k = 0; k < polls; ++k)
    {
        for (int i = 0; i < cmd_cnt; ++i)
        {
            kvs_send(fd, input[i], strlen(input[i]), 0);
            
            int ret = recv(fd, buffer, sizeof buffer - 1, 0);
            if (ret == -1)
            {
                LOG_SYSERR("recv() fails");
                abort();
            }

            buffer[ret] = 0;
            if (strcmp(buffer, output[i]) != 0)
            {
                printf("error: ---> %s", input[i]);
                printf("<---------- %s", buffer);
                printf("expected: %s", output[i]);
            }

        }
        ++request_cnt;
    }

    gettimeofday(&end, NULL);

    uint64_t total_time = (end.tv_sec - begin.tv_sec) * 1000 
        + (end.tv_usec - begin.tv_usec) / 1000;
    int qps = 0;
    if (total_time != 0)
    {
        qps = request_cnt * cmd_cnt / total_time * 1000;
    }
    
    printf("=========================\n");
    printf("request_cnt: %d, total_time(ms): %ld, qps: %d\n", request_cnt * cmd_cnt,
        total_time, qps);
    return 0;
}
