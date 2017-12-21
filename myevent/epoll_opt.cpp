#include "epoll_opt.h"
#include "event_interface.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
void epoll_add(BaseEvent *base, Event *ev)
{
    struct epoll_event evp;
    evp.data.fd = ev->fd;
    evp.events =
    epoll_ctl(base->epopt.fd, EPOLL_CTL_ADD, ev->fd, &evp);
}

void epoll_del(BaseEvent *base, Event *ev)
{
    struct epoll_event evp;
    evp.data.fd = ev->fd;
    evp.events =
    epoll_ctl(base->epopt.fd, EPOLL_CTL_DEL, ev->fd, &evp);
}

void epoll_mod(BaseEvent *base, Event *ev)
{
    struct epoll_event evp;
    evp.data.fd = ev->fd;
    evp.events =
    epoll_ctl(base->epopt.fd, EPOLL_CTL_MOD, ev->fd, &evp);
}

void epoll_dispatch(BaseEvent *base)
{
    int epfd = base->epopt.fd;
    int i;
    int ifd;
    struct epoll_event events[];
    int ready = epoll_wait(epfd, events, 1000, getmintimeout());
    for(i = 0; i < ready; ++i)
    {
        ifd = events[i].data.fd;
        if(events[i].events & EPOLLIN)
        {
            base->ev_map[ifd].event =
        }
        if(events[i].events & EPOLLOUT)
        {
            base->ev_map[ifd].event =
        }
        if(events[i].events & (EPOLLERR | EPOLLHUP))
        {
            base->ev_map[ifd].event =
        }
        add_list();
    }
}
