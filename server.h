#pragma once

#include <stdio.h>

#define BUFFER_LENGTH 4096
#define CONNECTION_LENGTH 1024
#define READY_LENFTH 1024
#define PORT_NUM 1

#define NETWORK_REACTOR

typedef int (*RCallBack)(int fd);

typedef struct connection connection;
typedef void (*ConnectionCallbackFunc)(struct connection *conn);


// io事件的回调函数
typedef struct EventHandler {
    int (*request_handler)(struct connection *conn);
	int (*response_handler)(struct connection *conn);
    int (*error_handler)(struct connection *conn);
	int (*close_handler)(struct connection *conn);
	int (*accept_handler)(struct connection *conn);
} EventHandler;  


typedef enum {
    CONN_STATE_NONE = 0,
    CONN_STATE_CONNECTING,
    CONN_STATE_ACCEPTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_CLOSED,
    CONN_STATE_ERROR
} ConnectionState;

// socket连接
struct connection
{
    int fd;
    char rbuffer[BUFFER_LENGTH];
    char wbuffer[BUFFER_LENGTH];
    int rlength;
    int wlength;
	EventHandler *handler;
	int (*recv_callback)(int);
	int (*send_callback)(int);
    ConnectionState status;
	void *private_data;
	int last_errno;
};

int set_event(int fd, int event, int flag);

void error_handling(const char *message);

void log_error(const char *message);

void server_start(int port, EventHandler *handler);