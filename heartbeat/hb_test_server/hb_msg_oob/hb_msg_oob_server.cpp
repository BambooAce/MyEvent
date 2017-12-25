#include "hb_msg_oob_server.h"
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
const char * hb = "HEARTBEAT";
const char * OOB_HB = "Q";
const char * OOB_ACK = "A";
static void setnonblock(int lisfd)
{
    int flags = fcntl(lisfd, F_GETFL);
    if(flags)
    {
        fcntl(lisfd, F_SETFL, O_NONBLOCK | flags);
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
    //setkeepalive(lisfd, 5, 3, 10);
    if(listen(lisfd, 100) != 0)
    {
        return -1;
    }
    return lisfd;
}

void loop(struct BaseEvent *base,int lisfd)
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
    time_t cache;
    int n;
    int maxfd  = -1;
    while(1)
    {
    lable:
        int ready = epoll_wait(epfd, events, MAXCLIENT, 300);
        if(ready == -1)
        {
            if(errno == EINTR)
                goto lable;
        }
        time_t now = time(NULL);
        cache = now;
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
                        maxfd = clifd > maxfd ? clifd : maxfd;
                        setnonblock(clifd);
                        ev.data.fd = clifd;
                        ev.events = EPOLLIN | EPOLLPRI | EPOLLERR;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &ev);
                        base->cli_map[clifd].fd = clifd;
                        base->cli_map[clifd].idle = now;
                        base->cli_map[clifd].times = 0;
                        base->cli_map[clifd].interval= 20;
                        base->cli_map[clifd].flags = CONNECTED;
                        (base->cliNum)++;
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
                        write(events[i].data.fd, buff, rd);
                        base->cli_map[events[i].data.fd].idle = cache;
			base->cli_map[events[i].data.fd].times = 0;
                        memset(buff, 0, SIZE);
                    }
                    if (rd == 0)
                    {
                        fprintf(stderr, "client close normally\n" );
                        close(events[i].data.fd);
                        memset(&(base->cli_map[events[i].data.fd]),0, sizeof(struct Event));
                    }
                }
                else if(events[i].events & EPOLLPRI)
                {
                    size_t oob_msg = recv(events[i].data.fd, buff, SIZE, MSG_OOB);
                    if (oob_msg)
                    {
                            if (strcmp(buff, OOB_ACK) == 0)
                            {
                                fprintf(stderr, "Recv MSG_OOB\n");
                            }
                            base->cli_map[events[i].data.fd].idle = cache;
                            base->cli_map[events[i].data.fd].times = 0;
                            memset(buff, 0, SIZE);
                    }
                }
            }
        }
        for (n = 0; n <= maxfd; ++n)
        {
            if ((base->cli_map[n].flags == CONNECTED) && (cache >= (base->cli_map[n].idle +base->cli_map[n].interval)) && (base->cli_map[n].times < 3))
            {
                send(base->cli_map[n].fd, OOB_HB, strlen(OOB_HB), MSG_OOB);
                base->cli_map[n].idle = cache;
                (base->cli_map[n].times)++;
            }
            else if (base->cli_map[n].times == 3)
            {
                fprintf(stderr, "%d may be offline  \n", base->cli_map[n].fd);
                base->cli_map[n].flags = OFFLINE;
                base->cli_map[n].times = 0;
                ev.events = EPOLLIN;
                ev.data.fd = base->cli_map[n].fd;
                epoll_ctl(epfd, EPOLL_CTL_DEL, base->cli_map[n].fd , &ev);
                close(base->cli_map[n].fd);
                //memset(&(base->cli_map[i]), 0, sizeof(struct Event));
            }
        }
    }
}

int main()
{
    struct BaseEvent base;
    base.cliNum = 0;
    base.cli_map = (struct Event*)malloc(sizeof(struct Event)*MAXCLIENT);
    memset(base.cli_map, 0, sizeof(struct Event) * MAXCLIENT);
    int port = 6666;
    int lisfd = initListenfd(port);
    loop(&base,lisfd);
    return 0;
}
