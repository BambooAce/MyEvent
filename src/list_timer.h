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
#ifdef THREADS
#include <pthread.h>
#endif
enum {CONNECTED, READ, WROTE, NORMAL};
struct util_time;
typedef struct ClientData{
	int fd;
	char ipaddr[4];
	char * data;
    //This client monitor status
    int flags;
    //Timer point, if ut is null that mean not included into timer list.
    struct util_time * ut;
}CLIENTDATA;

typedef struct util_time{
	struct  util_time * prev;
	struct util_time * next;
    //Client data point
	CLIENTDATA * cdata;
    //timeout value
	time_t out_time;
    //if timeout callback
    int (*timeout_callback)(CLIENTDATA * data);
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
#ifdef THREADS
    pthread_mutex_t mutex;
#endif
}LIST_TIMER;

//timer list operator, all function only handle list and free timer, not free clientdata
int add_timer(LIST_TIMER * lt, UTIL_TIME * ut);
int del_timer(LIST_TIMER * lt, UTIL_TIME **ut);
int adjust_timer(LIST_TIMER * lt, UTIL_TIME * ut, time_t _time);
void tick(LIST_TIMER * lt);
void init_List_Timer(LIST_TIMER * lt);
void destroy_list_Timer(LIST_TIMER * lt);
#endif
