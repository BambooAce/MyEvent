#ifndef ROUND_TIMER_H
#define ROUND_TIMER_H
#include <time.h>

typedef struct aw_timer{
    void *data;
    struct aw_timer * prev;
    struct aw_timer * next;
    time_t expect;
    //current slot number
    int slotnumber;
    //timer loop cercle number
    int loopcercle;
    //weather persist
    int persist;
    //if return 0 : mean don`t delete timer
    //return 1 : delete timer.
    void (*timeout_callback)(void * data);
}AW_TIMER;

#define N 60
typedef struct wh_timers{
    //time gap 1 sec or 1 min or 1 hour
    int timegap;
    //current slot point locetion
    int curslotpoint;
    //timer list point
    AW_TIMER *slot[N];
}WH_TIMERS;

void init_wh_timer(WH_TIMERS * wt, int timegap);
int add_wh_timer(WH_TIMERS * wt, AW_TIMER *at);
int del_wh_timer(WH_TIMERS * wt, AW_TIMER **at, int remove);
int adjust_wh_timer(WH_TIMERS * wt, AW_TIMER *at);
void wh_tick(WH_TIMERS * wt);
void destory_wh_timer(WH_TIMERS * wt);

#endif
