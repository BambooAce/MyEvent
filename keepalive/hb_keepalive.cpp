#include "hb_keepalive.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define MAXCLIENT 50000
#define SIZE 256

static void setnonblock(int lisfd)
{
    int flags = fcntl(lisfd, F_GETFL);
    if(flags)
    {
        fcntl(lisfd, F_SETFL, O_NONBLOCK | flags);
    }
}

static size_t Write(int fd, void *buff, size_t len)
{
    size_t nlen = 0;
    void *point = buff;
    size_t n = 0;
    while(1)
    {
        n = write(fd, point, len - nlen);
        if(n == -1)
        {
            if(errno != EAGAIN)
                continue;
            else
                return nlen;
        }
        else{
            nlen += n;
            if(nlen == len)
            {
                return nlen;
            }else{
                point += nlen;
                continue;
            }
        }
    }
}

static size_t Read(int fd, void *buff, size_t len)
{
    size_t nlen = 0;
    void *point = buff;
    size_t n = 0;
    while(1)
    {
        n = read(fd, point, len - nlen);
        if(n == -1)
        {
            if(errno != EAGAIN)
                continue;
            else
                return nlen;
        }
        else{
            if(n == 0)
            {
                close(fd);
                return nlen;
            }
            nlen += n;
            if(nlen == len)
            {
                return nlen;
            }else{
                point += nlen;
                continue;
            }
        }
    }
}

int initListenfd(int port)
{
    int lisfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lisfd < 0)
    {
        return -1;
    }
    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if(bind(lisfd, (const struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        return -1;
    }
    char re = 1;
    setsockopt(lisfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&re, re);
    setsockopt(lisfd, SOL_SOCKET, SO_REUSEPORT, (const void *)&re, re);
    setnonblock(lisfd);
    setkeepalive(lisfd, 10, 2, 10);
    if(listen(lisfd, 100) != 0)
    {
        return -1;
    }
    return lisfd;
}

void setkeepalive(int lisfd, unsigned int begin, unsigned int cnt, unsigned int intvl)
{
    if(lisfd){
        int keepalive = 1;
        if(setsockopt(lisfd, SOL_SOCKET, SO_KEEPALIVE,(const void *)&keepalive, sizeof(keepalive)) == -1)
        {
            fprintf(stderr, "SO_KEEPALIVE %s\n", strerror(errno));
        }
        if(setsockopt(lisfd, IPPROTO_TCP, TCP_KEEPIDLE, (const void *)&begin, sizeof(begin)) == -1)
        {
            fprintf(stderr, "TCP_KEEPIDLE %s\n", strerror(errno));
        }
        if(setsockopt(lisfd, IPPROTO_TCP, TCP_KEEPCNT, (const void *)&cnt, sizeof(cnt))==-1)
        {
            fprintf(stderr, "TCP_KEEPCNT %s\n", strerror(errno));
        }
        if(setsockopt(lisfd, IPPROTO_TCP, TCP_KEEPINTVL, (const void *)&intvl, sizeof(intvl))==-1)
        {
            fprintf(stderr, "TCP_KEEPINTVL %s\n", strerror(errno));
        }
    }
}

void loop(int lisfd)
{
    int epfd = epoll_create(MAXCLIENT);
    if(epfd < 0)
        return;
    struct epoll_event events[MAXCLIENT];
    memset(events, 0, sizeof(struct epoll_event) * MAXCLIENT);
    struct epoll_event ev;
    ev.data.fd = lisfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lisfd, &ev);
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int clifd = -1;
    char buff[SIZE] = {0};
    while(1)
    {
    lable:
        int ready = epoll_wait(epfd, events, MAXCLIENT, -1);
        if(ready == -1)
        {
            if(errno == EINTR)
                goto lable;
        }
        if(ready)
        {
            int i;
            for(i = 0;i < ready; ++i)
            {
                if(events[i].data.fd == lisfd)
                {
                lable2:
                    while((clifd = accept(lisfd, (struct sockaddr *)&cliaddr, &len)) > 0)
                    {
                        setnonblock(clifd);
                        
                        ev.data.fd = clifd;
                        ev.events = EPOLLIN | EPOLLPRI | EPOLLERR;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &ev);
                    }
                    if(clifd == -1)
                    {
                        if(errno != EAGAIN){
                            goto lable2;
                        }
                    }
                }else if(events[i].events & EPOLLIN)
                {
                    size_t rd = read(events[i].data.fd, buff, SIZE);
                    if(rd)
                    {
                        printf("%s\n", buff);
                        memset(buff, 0, SIZE);
                        write(events[i].data.fd, buff, rd);
                    }
                    if (rd == 0)
                    {
                        fprintf(stderr, "closed\n");
                        //close(events[i].data.fd);
                        //epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &(events[i]));
                    }
                    if (rd == -1)
                    {
                        if (errno ==ECONNRESET )
                        {
                        fprintf(stderr, "connect reset\n");
                        }
                        else if (errno == EHOSTUNREACH)
                        {
                            fprintf(stderr, "host unreach\n");
                        }
                        else if (errno ==ETIMEDOUT )
                        {
                            fprintf(stderr, "timeout\n");
                        }
                    }
                }
                else if(events[i].events & EPOLLPRI)
                {
                    //
                }
                else if(events[i].events & EPOLLOUT)
                {
                    //
                }
                else if(events[i].events & EPOLLERR)
                {
                    fprintf(stderr, "have error\n" );
                }
            }
        }
    }
}

int main()
{
    int port = 6666;
    int lisfd = initListenfd(port);
    loop(lisfd);
    return 0;
}
