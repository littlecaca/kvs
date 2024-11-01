#include "reactor.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/signal.h>

#include "server.h"
#include "logger.h"
#include "kvs_malloc.h"

static connection *conn_list;
static size_t conn_len;
static int epfd;
static int client_cnt = 0;
static struct timeval last_time = {};
static EventHandler *event_handler;

static void error_handling(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno));   
    exit(1);
}

static void log_error(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno)); 
}

/**
 * Extend connection list if necessay.
 * @return -1, invalid fd; -2 conn_len has reach limit; -3 out of memory.
 */
static int try_prepare_list(int fd)
{
    if (fd < 0)
        return -1;

    while (fd >= conn_len)
    {
        if (conn_len * 2 > CONNECTION_MAX_NUM)
            return -2;

        connection *new_list = ktryrealloc(conn_list, conn_len * 2 * sizeof(connection));
        if (new_list == NULL)  // oom
            return -3;
        conn_list = new_list;
        conn_len *= 2;
    }
    return 0;
}


int init_listen(unsigned short port)
{
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) error_handling("socket() fails");
    
    struct sockaddr_in serv_addr = {0};
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	serv_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof serv_addr) == -1)
    {
        error_handling("bind() fails");
    }
    
    if (listen(server_fd, 10) == -1)
    {
        error_handling("listen() fails");
    }
    return server_fd;
}

int set_event(int fd, int event, int flag)
{
    struct epoll_event et;
    et.events = event;
    et.data.fd = fd;
    while (1)
    {
	    int res = epoll_ctl(epfd, flag, fd, &et);
    	if (res == -1)
    	{
	        if (errno == EINTR || errno == EAGAIN)
                continue;
            error_handling("epoll_ctl() fails");
            return -1;
	    }
	    else break;
    }
    return 0;
}

void closefd(connection *pconn)
{
    // close
    if (pconn->handler->close_handler)
    {
        pconn->handler->close_handler(pconn);
    }
    set_event(pconn->fd, 0, EPOLL_CTL_DEL);
    if (close(pconn->fd) == -1) error_handling("close() fails");
}

int accept_cb(int fd);
int send_cb(int fd);

int recv_cb(int fd)
{
    connection *pconn = conn_list + fd;
    int clnt_fd = fd;

    int read_len = recv(clnt_fd, pconn->rbuffer, BUFFER_LENGTH - 1, 0);

    if (read_len == 0)
    {
        closefd(pconn);
        return 0;
    }

    if (read_len == -1) 
    {
        set_event(clnt_fd, 0, EPOLL_CTL_DEL);
        if (close(clnt_fd) == -1) error_handling("close() fails");
        log_error("recv fails()");
        return 0;
    }

    pconn->rlength = read_len;
    pconn->rbuffer[read_len] = '\0';

    if (pconn->handler->request_handler)
        pconn->handler->request_handler(pconn);
    
    if (pconn->wbuffer > 0)
    {
        set_event(pconn->fd, EPOLLIN | EPOLLOUT, EPOLL_CTL_MOD);
    }
    return 0;
}

int registerFd(int fd, int flag)
{
    int ret = try_prepare_list(fd);
    if (ret < 0)
    {
        if (ret == -2)
        {
            LOG_ERROR("The connetion number has reach limit.");
        }
        return -1;
    }

    connection *pconn = conn_list + fd;
    memset(pconn, 0, sizeof (connection));
    pconn->fd = fd;
    pconn->send_callback = send_cb;
    pconn->handler = event_handler;

    if (flag)
    {
        pconn->recv_callback = accept_cb;
    }
    else
    {
        pconn->recv_callback = recv_cb;
        if (pconn->handler->accept_handler)
        {
            pconn->handler->accept_handler(&conn_list[fd]);
        }
    }

    set_event(fd, EPOLLIN, EPOLL_CTL_ADD);
    return 0;
}

int send_cb(int fd)
{
    connection *pconn = conn_list + fd;

    if (pconn->handler->response_handler)
        pconn->handler->response_handler(pconn);

    if (pconn->status == CONN_STATE_CLOSED)
    {
        closefd(pconn);
    }
    else
    {
        int clnt_fd = pconn->fd;

        if (pconn->wlength > 0)
        {
            send(clnt_fd, pconn->wbuffer, pconn->wlength, 0);
        }
        
        set_event(clnt_fd, EPOLLIN, EPOLL_CTL_MOD);
    }
    return 0;
}

int accept_cb(int fd)
{
    struct sockaddr_in clnt_addr = {0};
    socklen_t clnt_addr_size = sizeof (struct sockaddr_in);
    int clnt_fd = accept(fd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_fd == -1)
    {
        error_handling("accept() fails");
    }
    registerFd(clnt_fd, 0);

    if (++client_cnt % 1000 == 0)
    {
        struct timeval now;
        gettimeofday(&now, NULL);

        int time_used = (now.tv_sec - last_time.tv_sec) * 1000 +  (now.tv_usec - last_time.tv_usec) / 1000;
        last_time = now;
        printf("totoal client count: %d, time used: %d\n", client_cnt, time_used);
    }
    return 0;
}

static inline void create_connection_list()
{
    assert(conn_list == NULL);
    conn_list = kmalloc(sizeof(connection) * CONNECTION_MIN_NUM);
    conn_len = CONNECTION_MIN_NUM;
}

static inline void destroy_connection_list()
{
    assert(conn_list != NULL);
    kfree(conn_list);
    conn_len = 0;
}

// server
int reactor_start(int port, EventHandler *handler)
{
    // ingore signal SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    create_connection_list();

    epfd = epoll_create1(0);
    event_handler = handler;

    int server_fd;
    for (int i = 0; i < PORT_NUM; ++i)
    {
        server_fd = init_listen(port + i);
        registerFd(server_fd, 1);
    }

    gettimeofday(&last_time, NULL);
    
    struct epoll_event events[READY_LENFTH] = {0};
    int nready;
    
    while (1)
    {
        nready = epoll_wait(epfd, events, READY_LENFTH, 5);
        if (nready == -1)
        {
			if (errno == EINTR || errno == EAGAIN) continue;
            error_handling("epoll_wait() fails");
        }

        for (int i = 0; i < nready; ++i)
        {
            int clnt_fd = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                conn_list[clnt_fd].recv_callback(clnt_fd);
            }
            if (events[i].events & EPOLLOUT)
            {
                conn_list[clnt_fd].send_callback(clnt_fd);
            }
        }
    }
    
    destroy_connection_list();
    return 0;
}
 