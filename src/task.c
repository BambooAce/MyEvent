#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/**
 * @brief initTaskList
 * @param tl
 * @return
 * Initial task list and lock
 */
int initTaskList(TASKLIST *tl)
{
    tl->header = NULL;
    tl->num = 0;
    pthread_mutex_init(tl->mutex, NULL);
    return 1;
}

/**
 * @brief getATask
 * @param tl
 * @param tpkg
 * @return
 * get a task from tasklist, put into tpkg, if success return 1, else return 0.
 */
int getATask(TASKLIST *tl, TASKPACKAGE * tpkg)
{
    if(tl){
        pthread_mutex_lock(tl->mutex);
        if(tl->num)
        {
            TaskNode *tn = tl->header;
            if(tn)
            {
                tl->header = tn->next;
                tpkg->arg = tn->tg->arg;
                tpkg->et = tn->tg->et;
                tpkg->status = 1;
                free(tn);
                tn = NULL;
                pthread_mutex_unlock(tl->mutex);
                return 1;
            }
        }
        pthread_mutex_unlock(tl->mutex);
    }
    return 0;
}

/**
 * @brief putATask
 * @param tl
 * @param runtask callback function
 * @param arg argurement
 * @param argsize size of argurement
 * @return
 * put a task into tasklist
 */
int putATask(TASKLIST *tl, ExecuteTask runtask, void *arg, unsigned int argsize)
{
    assert(tl);
    TASKPACKAGE * tp = (TASKPACKAGE *)malloc(sizeof(TASKPACKAGE));
    if(tp && arg){
        memcpy(tp->arg, arg, argsize);
    }
    else if(tp && !arg){
        tp->arg = NULL;
    }else
    {
        return 0;
    }
    tp->et = runtask;
    tp->status = 0;
    TaskNode *atask = (TaskNode *)malloc(sizeof(TaskNode));
    if(atask){
        atask->tg = tp;
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

/**
 * @brief clearTaskList
 * @param tl
 * @return
 * clean tasklist
 */
void clearTaskList(TASKLIST *tl)
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
}

/**
 * @brief deleteTask
 * @param tl
 * @param _tg
 * @return
 * delete all tg tasks
 */
void deleteTask(TASKLIST *tl, ExecuteTask _tg)
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
}

/**
 * @brief getTaskNum
 * @param tl
 * @return
 * get task number
 */
int getTaskNum(TASKLIST *tl)
{
    pthread_mutex_lock(tl->mutex);
    int num = tl->num;
    pthread_mutex_unlock(tl->mutex);
    return num;
}
