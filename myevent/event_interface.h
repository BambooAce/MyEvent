#ifndef EVENT_INTERFACE_H
#define EVENT_INTERFACE_H
#include <time.h>
struct BaseEvent;
struct Event{
    int fd;
    time_t lastRecvTime;
    int flag;
    int hb;
    unsigned int hbTimes;
    struct BaseEvent *be;
    void (*callback)();
    void *argv;
};

struct BaseEvent{
    unsigned int hbtimegap;
    struct Event *ev_map;
    unsigned int evNum;
    struct opt epopt;
};
#endif // EVENT_INTERFACE_H
