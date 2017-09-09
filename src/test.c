#include "task.h"
#include "pthreadPool.h"
#include <stdio.h>

void runtask(void *arg);
void runtask2(void *arg);

int main()
{
    int temp = 100;
    int temp2 = 10;
    int temp3 = 1;
    TASKLIST tl;
    initTaskList(&tl);

    putATask(&tl, runtask, &temp, sizeof(temp));
    putATask(&tl, runtask, &temp2, sizeof(temp2));
    putATask(&tl, runtask, &temp3, sizeof(temp3));
    sleep(2);
    initThreadPool(10, &tl, 1);
    joinAllThreads();
    return 0;
}

void runtask(void *arg)
{
    int *a = (int *)arg;
    while(1)
    {
        printf(">>>%d\n", (*a)++);
        sleep(1);
    }
}
