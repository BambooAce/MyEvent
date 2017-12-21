#ifndef EPOLL_OPT_H
#define EPOLL_OPT_H
struct Event;
struct BaseEvent;
void epoll_add(struct BaseEvent *base, struct Event *ev);
void epoll_del(struct BaseEvent *base, struct Event *ev);
void epoll_mod(struct BaseEvent *base, struct Event *ev);
void epoll_dispatch(struct BaseEvent *base);

#endif // EPOLL_OPT_H
