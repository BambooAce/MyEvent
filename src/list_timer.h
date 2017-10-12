/**
 * timer list
 *
 *
 *
 *
 *
 */
#ifndef LIST_TIMER_H
#define LIST_TIMER_H
#include <time.h>

typedef struct util_time{
	struct  util_time * prev;
	struct util_time * next;
    //Client data point
    void * cdata;
    //timeout value
    time_t out_time;
    int persist;
    //if timeout callback
    void (*timeout_callback)(void * data);
}UTIL_TIME;

struct list_timer;

struct TimerOP
{
    int (*add)(struct list_timer * lt, UTIL_TIME * ut);
    int (*del)(struct list_timer * lt, UTIL_TIME ** ut);
    int (*adjust)(struct list_timer * lt, UTIL_TIME * ut, time_t _time);
    void (*_tick)(struct list_timer * lt);
};

//Timer list
typedef struct list_timer
{
    struct TimerOP timer_op;
	UTIL_TIME * head;
	UTIL_TIME * tail;
}LIST_TIMER;

//timer list operator, all function only handle list and free timer, not free clientdata
int add_timer(LIST_TIMER * lt, UTIL_TIME * ut);
int del_timer(LIST_TIMER * lt, UTIL_TIME **ut);
int adjust_timer(LIST_TIMER * lt, UTIL_TIME * ut, time_t _time);
void tick(LIST_TIMER * lt);
void init_List_Timer(LIST_TIMER * lt);
void destroy_list_Timer(LIST_TIMER * lt);
#endif
