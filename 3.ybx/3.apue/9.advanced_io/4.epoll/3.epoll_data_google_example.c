#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define DEBUG 0

#define SERVER_BUF_LEN 255
#define LISTEN_PORT  8080
#define LISTEN_QUEUE 1000
#define EPOLL_SIZE_HINT 10000
#define EPOLL_EVENT_QUEUE 100

typedef struct {
    int fd;
    int w_bytes;
    int r_done;
} conn_t;

typedef struct {
    int lsock;
    int epoll;
    struct epoll_event *ev_buf;
    char *buf;
    char *response;
    int rlen;
    int *conn;
    int open;
    int close;
} server_t;

void error(const char *msg);

int new_lsock (void);
int new_epoll (void);

void ini_server (server_t *server);
void run_server (server_t *server);

void proc_ev (server_t *server, int num_ev);

void acpt_conn (server_t *server);
void proc_conn (server_t *server, struct epoll_event *ev);

void read_conn (server_t *server, struct epoll_event *ev);
void writ_conn (server_t *server, struct epoll_event *ev);
void eror_conn (server_t *server, struct epoll_event *ev);
void clos_conn (server_t *server, struct epoll_event *ev);

//http://ersun.warnckes.com/code/c/server.html
int main (void)
{
    printf("STARTING SERVER\n");
    server_t *server = (server_t *)malloc(sizeof(server_t));

    server->lsock = new_lsock();
    server->epoll = new_epoll();

    ini_server(server);
    run_server(server);

    return 0;
}

void ini_server (server_t *server)
{
    server->open  = 0;
    server->close = 0;
    server->ev_buf = (struct epoll_event *)malloc(sizeof(struct epoll_event)*EPOLL_EVENT_QUEUE);
    //if (DEBUG) printf("EV BUF SIZE: %lu\n", sizeof(struct epoll_event)*EPOLL_EVENT_QUEUE);
    server->buf    = (char *)malloc(sizeof(char) * (SERVER_BUF_LEN+1));
    server->response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 11\r\n"
        "Connection: close\r\n\r\n"
        "Hello World";
    server->rlen = strlen(server->response);

    conn_t *conn = (conn_t *)malloc(sizeof(conn_t));
    conn->fd = server->lsock;

    struct epoll_event ev;

    ev.data.ptr = conn;
    ev.events   = EPOLLIN;

    if (epoll_ctl(server->epoll, EPOLL_CTL_ADD, server->lsock, &ev) == -1)
        error("EPOLL_CTL");
}

void run_server (server_t *server)
{
    int num_ev;

    for (;;) {
        num_ev = epoll_wait(server->epoll, server->ev_buf, EPOLL_EVENT_QUEUE, 10);
        switch (num_ev) {
            case  0: break;
            case -1: error("EPOLL_WAIT");
            default:
                ////if (DEBUG) printf("EPOLL: %d\n", num_ev);
                proc_ev(server, num_ev);
        }
    }
}

void proc_ev (server_t *server, int num_ev)
{
    int i;
    struct epoll_event *ev;

    for (i=0; i < num_ev; i++) {
        ////if (DEBUG) printf("i: %d; off: %lu\n", i, sizeof(struct epoll_event) * i);
        ev = server->ev_buf + sizeof(struct epoll_event) * i;
        if (! ev->data.ptr) continue;
        if ((long long unsigned int)ev->data.ptr > 0xffffffff) continue;
        if ( ((conn_t *)ev->data.ptr)->fd == server->lsock )
            acpt_conn(server);
        else
            proc_conn(server, ev);
    }
}

void acpt_conn (server_t *server)
{
    int asock;
    struct sockaddr_in clnt_addr;
    socklen_t socklen;

    //if (DEBUG) printf("ENTER ACCEPT\n");

    while ( (asock = accept4(server->lsock, (struct sockaddr *)&clnt_addr, &socklen, SOCK_NONBLOCK)) > 0) {
        (server->open)++;
        //if (DEBUG) printf("ACCEPT %d\n", server->open);
        conn_t *conn = (conn_t *)malloc(sizeof(conn_t));
        if (! conn)
            error("MALLOC");
        conn->w_bytes = 0;
        conn->fd = asock;
        conn->r_done = 0;

        struct epoll_event ev;

        ev.data.ptr = (void *)conn;
        ev.events   = EPOLLIN;

        if (epoll_ctl(server->epoll, EPOLL_CTL_ADD, asock, &ev) == -1)
            error("EPOLL_CTL");
    }
    //if (DEBUG) printf("EXIT ACCEPT\n");
}

void proc_conn (server_t *server, struct epoll_event *ev)
{
    //if (DEBUG) printf("PROC CONN %p\n", ev);
    if (ev->events & EPOLLIN) {
        //if (DEBUG) printf("READ CONN\n");
        read_conn(server, ev);
    }
    else if (ev->events & EPOLLOUT) {
        //if (DEBUG) printf("WRITE CONN\n");
        writ_conn(server, ev);
    }
    else if (ev->events & EPOLLERR) {
        //if (DEBUG) printf("EROR CONN\n");
        eror_conn(server, ev);
    }
    else if (ev->events & EPOLLHUP) {
        //if (DEBUG) printf("CLOS CONN\n");
        clos_conn(server, ev);
    }
    //if (DEBUG) printf("DONE PROC CONN\n");
}

void read_conn (server_t *server, struct epoll_event *ev)
{
    conn_t *conn = (conn_t *)ev->data.ptr;
    if (! conn) {
        epoll_ctl(server->epoll, EPOLL_CTL_DEL, conn->fd, NULL);
        shutdown(conn->fd, SHUT_RDWR);
        close(conn->fd);
    }
    //if (DEBUG) printf("START READING %p\n", conn);
    int b;
    while ( (b = read(conn->fd, server->buf, SERVER_BUF_LEN)) > 0 ) {
        //if (DEBUG) printf("READ %d\n", b);
        ev->events = EPOLLOUT;
        epoll_ctl(server->epoll, EPOLL_CTL_MOD, conn->fd, ev);
        conn->r_done = 1;
    }
    if (b == 0 && conn->r_done == 0) {
        epoll_ctl(server->epoll, EPOLL_CTL_DEL, conn->fd, NULL);
        shutdown(conn->fd, SHUT_RDWR);
        close(conn->fd);
        free(conn);
        (server->close)++;
        //if (DEBUG) printf("CLOSE %d\n", server->close);
    }
}

void writ_conn (server_t *server, struct epoll_event *ev)
{
    conn_t *conn = (conn_t *)ev->data.ptr;
    if (! conn->r_done) return;
    int w_bytes = write(conn->fd, server->response, server->rlen);
    if (w_bytes < 0)
        return;
    //if (DEBUG) printf("WRITE %d\n", w_bytes);
    if (conn->w_bytes + w_bytes == server->rlen) {
        epoll_ctl(server->epoll, EPOLL_CTL_DEL, conn->fd, NULL);
        shutdown(conn->fd, SHUT_RDWR);
        close(conn->fd);
        free(conn);
        (server->close)++;
    }
    else {
        conn->w_bytes += w_bytes;
    }
}

void eror_conn (server_t *server, struct epoll_event *ev)
{
    //if (DEBUG) printf("ERROR");
}

void clos_conn (server_t *server, struct epoll_event *ev)
{
    //if (DEBUG) printf("CLOSE");
}

int new_epoll (void)
{
    int epoll = epoll_create(EPOLL_SIZE_HINT);
    if (epoll == -1)
        error("EPOLL");
    return epoll;
}

int new_lsock (void)
{
    int lsock, optval;
    struct sockaddr_in serv_addr;

    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0)
        error("SOCKET");

    optval = 1;
    if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        error("SETSOCKOPT");

    if (fcntl(lsock, F_SETFL, O_NONBLOCK) == -1)
        error("FCNTL");

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(LISTEN_PORT);

    if ( bind(lsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 )
        error("BIND");

    if ( listen(lsock, LISTEN_QUEUE) == -1 )
        error("LISTEN");

    return lsock;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

