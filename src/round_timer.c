#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "round_timer.h"
static WH_TIMERS wt;
static void insert_sort_timer(AW_TIMER ** aw_list, AW_TIMER **at)
{
    if(aw_list && at)
    {
        AW_TIMER * atr = *aw_list;
        while(atr){
            if(((*at)->loopcercle <= (atr->loopcercle)) && (!atr->prev)){
                (*at)->next = atr;
                atr->prev = *at;
                (*at)->prev = NULL;
                *aw_list = *at;
                //fprintf(stderr, "#######\n");
                return;
            }
            else if(((*at)->loopcercle >= (atr->loopcercle)) && (!atr->next)) {
                atr->next = *at;
                (*at)->prev = atr;
                (*at)->next = NULL;
                fprintf(stderr, "^^^^^^^^\n");
                return;
            }
            else if(((*at)->loopcercle <= (atr->loopcercle)) && atr->next && atr->prev){
                (*at)->next = atr;
                (*at)->prev = atr->prev;
                atr->prev->next = *at;
                atr->prev = *at;
                fprintf(stderr, "||||||||\n");
                return;
            }
            atr = atr->next;
        }
    }
}

void init_wh_timer(WH_TIMERS * wt, int timegap)
{
    if(wt){
        wt->curslotpoint = 0;
        wt->timegap = timegap ? timegap : 1;
        int i;
        for(i = 0; i < N; ++i){
            wt->slot[i] = NULL;
        }
    }
}

int add_wh_timer(WH_TIMERS *wt, AW_TIMER *at)
{
    if(wt && at){
        if(at->expect){
            at->loopcercle = (at->expect) / ((wt->timegap) * N);
            at->slotnumber = (wt->curslotpoint + ((at->expect) / (wt->timegap))) % N;
            //fprintf(stderr, "timer slot number: %d\n", at->slotnumber);
            int index_t = at->slotnumber;
            if(wt->slot[index_t])
            {
                //fprintf(stderr, ".................\n");
                insert_sort_timer(&(wt->slot[index_t]), &at);
            }else{
                wt->slot[index_t] = at;
                at->prev = NULL;
                at->next = NULL;
            }
        }
    }
}

int del_wh_timer(WH_TIMERS *wt, AW_TIMER **at, int remove)
{
    if(wt && at){
        if(*at){
            if(!((*at)->prev)){
                wt->slot[(*at)->slotnumber] = (*at)->next;
                if((*at)->next)
                    (*at)->next->prev = NULL;
            }
            else if(!((*at)->next))
            {
                if((*at)->prev)
                    (*at)->prev->next = NULL;
                else
                    wt->slot[(*at)->slotnumber] = NULL;
            }else{
                (*at)->prev->next = (*at)->next;
                (*at)->next->prev = (*at)->prev;
                (*at)->prev = NULL;
                (*at)->next = NULL;
            }
//            if(!remove){
//                free(*at);
//                *at = NULL;
//            }
        }
        return 0;
    }
    return 0;
}

int adjust_wh_timer(WH_TIMERS *wt, AW_TIMER *at)
{
    if(wt && at)
    {
        //at->expect *= 2;
        del_wh_timer(wt, &at, 0);
        add_wh_timer(wt, at);
    }
}

void wh_tick(WH_TIMERS *wt)
{
    if(wt)
    {
        wt->curslotpoint = (wt->curslotpoint + 1)%N;
        //fprintf(stderr, "wheet current point <<<<%d\n", wt->curslotpoint);

        if(wt->slot[wt->curslotpoint])
        {
            AW_TIMER * at = wt->slot[wt->curslotpoint];
            AW_TIMER *attemp = at;
            while(at)
            {
                attemp = at->next;
                if(0 >= at->loopcercle){
                    int res = at->timeout_callback(at->data);
                    switch(res){
                    case 0:
                        adjust_wh_timer(wt, at);
                        break;
                    case 1:
                        del_wh_timer(wt, &at, 1);
                        break;
                    default:
                        break;
                    }
                }
                at = attemp;
            }
        }
        alarm(wt->timegap);
    }
}

void destory_wh_timer(WH_TIMERS *wt)
{
    int i;
    for(i = 0; i < N; ++i)
    {
        if(wt->slot[i]){
            AW_TIMER * timer = wt->slot[i];
            AW_TIMER * temptimer = timer;
            while(timer)
            {
                temptimer = timer->next;
                free(timer);
                timer = NULL;
                timer = temptimer;
            }
            temptimer = NULL;
            wt->slot[i] = NULL;
        }
    }
}

int func(CLIENTDATA * idata)
{
    if(idata)
    {
        fprintf(stderr, "data is >>>>%d\n", idata->fd);
    }
    return 0;
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
    CLIENTDATA dt, dt2;
    dt2.fd = 111;
    dt.fd = 234;
    AW_TIMER at, at2;
    at.data = &dt;
    at.expect = 6;
    at.timeout_callback = func;
    at2.data = &dt2;
    at2.expect = 4;
    at2.timeout_callback = func;
    add_wh_timer(&wt, &at);
    add_wh_timer(&wt, &at2);
    //adjust_wh_timer(&wt, &at);
    //adjust_wh_timer(&wt, &at2);
    alarm(1);
    while(1){
    sleep(300);
    }
    return 0;
}
