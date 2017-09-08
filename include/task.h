#ifndef TASK_H
#define TASK_H
#include <pthread.h>

typedef void (*ExecuteTask)(void *arg);

typedef struct taskpackage{
    void * arg;
    unsigned int argsize;
    ExecuteTask et;
    int status; //0 in list no call, 1 get from list and running
}TASKPACKAGE;

typedef struct TASKNODE{
    TASKPACKAGE *tg;
    struct TASKNODE *pretask;
    struct TASKNODE *next;
}TaskNode;

typedef struct {
    TaskNode * header;
    unsigned int num;
    pthread_mutex_t  mutex;
}TASKLIST;

int initTaskList(TASKLIST *tl);
int getATask(TASKLIST *tl, TASKPACKAGE *tpkg);
int putATask(TASKLIST *tl, ExecuteTask runtask, void *arg, unsigned int argsize);
void deleteTask(TASKLIST *tl, ExecuteTask _tg);
void clearTaskList(TASKLIST *tl);
void getRunningTaskNum(TASKLIST * tl, int *tasknum);

#endif // TASK_H
