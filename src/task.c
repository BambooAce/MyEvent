#include "task.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int initTaskList(TASKLIST *tl)
{
    tl->header = NULL;
    tl->num = 0;
    pthread_mutex_init(tl->mutex, NULL);
    return 1;
}

TASKPACKAGE *getATask(TASKLIST *tl)
{
    assert(tl);
    if(tl->num)
    {
        TaskNode *tn = tl->header;
        TaskNode *tnn = tn;
        int i;
        for(i = 0; i < tl->num; ++i)
        {
            tnn = tn->next;
            tnn->priority ;
            tn = tnn;
        }
    }
    return NULL;
}

int putATask(TASKLIST *tl, ExecuteTask runtask, void *arg, unsigned int argsize, int priority)
{
    assert(tl);
    TASKPACKAGE * tp = (TASKPACKAGE *)malloc(sizeof(TASKPACKAGE));
    if(tp && arg){
        memcpy(tp->arg, arg, argsize);
    }else
        tp->arg = NULL;
    tp->et = runtask;
    tp->status = 0;
    TaskNode *atask = (TaskNode *)malloc(sizeof(TaskNode));
    if(atask){
        atask->tg = tp;
        atask->priority = priority > 0 ? priority : 1;
        pthread_mutex_lock(tl->mutex);
        if(tl->header){
            tl->header->pretask = atask;
            atask->next = tl->header;
            atask->pretask = NULL;
        }else{
            tl->header = atask;
            atask->pretask = NULL;
            atask->next = NULL;
        }
        tl->num++;
        pthread_mutex_unlock(tl->mutex);
        return 1;
    }
    return 0;
}


int clearTaskList(TASKLIST *tl)
{
    assert(tl);
    pthread_mutex_lock(tl->mutex);
    if(tl->num)
    {
        TaskNode * tn = tl->header;
        TaskNode * tnn = tn;
        while(tn && tl->num)
        {
            tnn = tn->next;
            if(tn->tg->arg){
                free(tn->tg->arg);
                tn->tg->arg = NULL;
            }
            if(tn->tg)
            {
                free(tn->tg);
                tn->tg = NULL;
            }
            free(tn);
            tn = tnn;
            tl->num--;
        }
    }
    tl->header = NULL;
    tl->num = 0;
    pthread_mutex_unlock(tl->mutex);
    return 1;
}

int deleteTask(TASKLIST *tl, ExecuteTask _tg)
{
    assert(tl);
    if(tl->num)
    {
        TaskNode *tn = tl->header;
        TaskNode *tnn = tn;
        while(tn && tl->num)
        {
            tnn = tn->next;
            if(tn->tg){
                if(tn->tg->et == _tg)
                {
                    if(!tn->pretask)
                    {
                        tl->header = tn->next;
                    }else{
                        tn->pretask->next = tn->next;
                        tn->next->pretask = tn->pretask;
                    }
                    if(tn->tg->arg){
                        free(tn->tg->arg);
                        tn->tg->arg = NULL;
                    }
                    free(tn->tg);
                    tn->tg = NULL;
                    free(tn);
                    tn = NULL;
                    tl->num--;
                    if(tl->num == 0)
                    {
                        tl->header = NULL;
                        break;
                    }
                }
            }
            tn = tnn;
        }
    }
    return 1;
}

int getTaskNum(TASKLIST *tl)
{
    return tl->num;
}
