#include <pthread.h>
typedef void (*ExecuteTask)(void **arg);

typedef struct taskpackage{
    void * arg;
    ExecuteTask et;
    int status;
}TASKPACKAGE;

typedef struct TASKNODE{
    int priority;
    TASKPACKAGE *tg;
    TASKNODE *pretask;
    TASKNODE *next;
}TaskNode;

typedef struct {
    TaskNode * header;
    unsigned int num;
    pthread_mutex_t * mutex;
}TASKLIST;

int initTaskList(TASKLIST *tl);
TASKPACKAGE * getATask(TASKLIST *tl);
int putATask(TASKLIST *tl, ExecuteTask runtask, void *arg, unsigned int argsize, int priority);
int deleteTask(TASKLIST *tl, ExecuteTask _tg);
int clearTaskList(TASKLIST *tl);
int getTaskNum(TASKLIST * tl);
