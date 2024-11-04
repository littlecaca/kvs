#pragma once

#include <stdio.h>

#define BUFFER_LENGTH (16 * 1024)
#define CONNECTION_MIN_NUM 3
#define CONNECTION_MAX_NUM (1024 * 1024)
#define READY_LENFTH 1024
#define PORT_NUM 1

#define NETWORK_REACTOR 1
#define NETWORK_NTYCO   2

#define NETWORK_SELECT NETWORK_NTYCO

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
	EventHandler *handler;
	int (*recv_callback)(int);
	int (*send_callback)(int);
	void *private_data;
    int fd;
    int rlength;
    int wlength;
    ConnectionState status;
	int last_errno;
    char rbuffer[BUFFER_LENGTH];
    char wbuffer[BUFFER_LENGTH];
};

void server_start(int port, EventHandler handler);
