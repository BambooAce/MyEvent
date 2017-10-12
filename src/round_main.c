#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "round_timer.h"

typedef struct ClientData{
    int fd;
    char ipaddr[4];
    char * data;
    //This client monitor status
    int flags;
    //Timer point, if ut is null that mean not included into timer.
    struct aw_timer * at;
}CLIENTDATA;

static WH_TIMERS wt;

void func(void * idata)
{
    if(idata)
    {
        CLIENTDATA * data = (CLIENTDATA *)idata;
        fprintf(stderr, "data is >>>>%d\n", data->fd);
    }
}

void doit(int status)
{
    fprintf(stderr, "time coming\n");
    wh_tick(&wt);
}

int main()
{
    signal(SIGALRM, doit);
    init_wh_timer(&wt, 1);
    CLIENTDATA *dt[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    int i;
    for(i = 0; i < 11; ++i)
    {
        dt[i] = (CLIENTDATA *)malloc(sizeof(CLIENTDATA));
        if(dt[i])
        {
            dt[i]->fd = i;
            dt[i]->data = NULL;
        }
    }
    AW_TIMER *at[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    for(i = 0; i < 11; ++i)
    {
        at[i] = (AW_TIMER *)malloc(sizeof(AW_TIMER));
        if(at[i])
        {
            at[i]->data = dt[i];
            at[i]->expect = i + 4;
            dt[i]->at = at[i];
            at[i]->timeout_callback = func;
            at[i]->persist = 1;
            add_wh_timer(&wt, at[i]);
        }
    }
    alarm(1);
    while(1){
        sleep(300);
    }
    for(i = 0; i < 11; ++i)
    {
        free(dt[i]);
        dt[i] = NULL;
    }
    return 0;
}
