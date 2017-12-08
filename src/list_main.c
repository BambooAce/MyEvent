#include "list_timer.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

enum {CONNECTED, READ, WROTE, NORMAL};

typedef struct ClientData{
    int fd;
    char ipaddr[4];
    char * data;
    //This client monitor status
    int flags;
    //Timer point, if ut is null that mean not included into timer list.
    struct util_time * ut;
}CLIENTDATA;
LIST_TIMER lt;

void doit(void * mydata)
{
    CLIENTDATA * data = (CLIENTDATA *)mydata;
    if(data){
        switch(data->flags){
        case CONNECTED:
            fprintf(stderr, "don`t delete\n");
            break;
        case READ:
            fprintf(stderr, "delete \n");
            break;
        case WROTE:
            break;
        case NORMAL:
            break;
        default:
            break;
        }
    }
}

int main()
{
    init_List_Timer(&lt);
    CLIENTDATA * cl1 = (CLIENTDATA *)malloc(sizeof(CLIENTDATA));
    cl1->data = NULL;
    cl1->fd = 23;
    cl1->flags = CONNECTED;

    CLIENTDATA * cl2 = (CLIENTDATA *)malloc(sizeof(CLIENTDATA));
    cl2->data = NULL;
    cl2->fd = 2324;
    cl2->flags = READ;

    UTIL_TIME *ut2 = (UTIL_TIME *)malloc(sizeof(UTIL_TIME));
    ut2->timeout_callback = doit;
    ut2->cdata = cl2;
    ut2->out_time = time(NULL) + 2;
    ut2->persist = 0;

    UTIL_TIME *ut1 = (UTIL_TIME *)malloc(sizeof(UTIL_TIME));
    ut1->timeout_callback = doit;
    ut1->cdata = cl1;
    ut1->out_time = time(NULL)+ 6;
    ut1->persist = 10;

    add_timer(&lt, ut1);
    add_timer(&lt, ut2);
    while(1){
        tick(&lt);
        usleep(500);
    }
    return 0;
}
