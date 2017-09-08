#include "task.h"
#include <unistd.h>
#define MAX 300
pthread_t Threads[MAX];
pthread_attr_t attr;
/**
 * @brief run
 * @param arg
 * @return
 * thread callback function, every thread get a task from task list
 * if get task then run it, or sleep
 * If you want get return value, you can convert argument into arg.
 */
static void * run(void *arg)
{
    TASKLIST *tl = (TASKLIST *)arg;
    if(tl)
    {
        TASKPACKAGE tkg;
        while(1){
            if(getATask(tl, &tkg))
            {
                tkg.et(tkg.arg);
            }else{
                usleep(50000);
            }
        }
    }
}

/**
 * @brief initThreadPool
 * @param threadnum
 * @param tl
 * @param detach
 * @return
 * Initial thread pool, you can choose detach or not.
 */
int initThreadPool(int threadnum, TASKLIST * tl, int detach)
{
    int i;
    for(i = 0; i < MAX; ++i)
    {
        Threads[i] = 0;
    }
    if(detach)
    {
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }
    for(i = 0;i < threadnum; ++i)
    {
        if( -1 == pthread_create(&Threads[i], (detach ? &attr: NULL), run, (void *)tl))
        {
            break;
        }
    }
    return 1;
}

/**
 * @brief joinAllThreads
 * @return
 * Wait all threads exit and collect status
 */
int joinAllThreads()
{
    int i;
    for(i = 0; i < MAX; ++i)
    {
        if(Threads[i] > 0)
        {
            pthread_join(Threads[i], NULL);
        }
    }
}
