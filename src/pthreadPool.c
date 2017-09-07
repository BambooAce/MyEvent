#include "task.h"
#include <unistd.h>
#define MAX 300
pthread_t Threads[MAX];

static void * run(TASKLIST *tl)
{
    if(tl)
    {
        TASKPACKAGE tkg;
        while(1){
            if(getATask(tl, &tkg))
            {
                tkg.et(tkg.arg);
            }else{
                usleep(800);
            }
        }
    }
}

int initThreadPool()
{
    int i;
    for(i = 0; i < MAX; ++i)
    {
        Threads[i] = 0;
    }
    for(;;)
    {
        pthread_create(&Threads[i], NULL, run, NULL);
    }
}
