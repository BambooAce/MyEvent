#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "round_timer.h"

/**
 * @brief insert_sort_timer
 * @param aw_list
 * @param at
 * sort timer depend on loopcercle
 */
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
                return;
            }
            else if(((*at)->loopcercle >= (atr->loopcercle)) && (!atr->next)) {
                atr->next = *at;
                (*at)->prev = atr;
                (*at)->next = NULL;
                return;
            }
            else if(((*at)->loopcercle <= (atr->loopcercle)) && atr->next && atr->prev){
                (*at)->next = atr;
                (*at)->prev = atr->prev;
                atr->prev->next = *at;
                atr->prev = *at;
                return;
            }
            atr = atr->next;
        }
    }
}

/**
 * @brief init_wh_timer
 * @param wt
 * @param timegap
 * Initialize wheel timers, timegap mean seconds one step.
 * If timegap < 0 , timegap = 1
 */
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

/**
 * @brief add_wh_timer
 * @param wt
 * @param at
 * @return
 * add a timer into wheel timers
 */
int add_wh_timer(WH_TIMERS *wt, AW_TIMER *at)
{
    if(wt && at){
        if(at->expect){
            //get timer loop cercles.
            at->loopcercle = (at->expect) / ((wt->timegap) * N);
            //get timer slot number in wheel
            at->slotnumber = (wt->curslotpoint + ((at->expect) / (wt->timegap))) % N;
            int index_t = at->slotnumber;
            if(wt->slot[index_t])
            {
                //If this slot is not empty, insert it sortable.
                insert_sort_timer(&(wt->slot[index_t]), &at);
            }else{
                wt->slot[index_t] = at;
                at->prev = NULL;
                at->next = NULL;
            }
        }
    }
}

/**
 * @brief del_wh_timer
 * @param wt
 * @param at
 * @param remove
 * @return
 * delete a timer, and the remove flag mean weather free it or not.
 */
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
            if(remove){
                free(*at);
                *at = NULL;
            }
        }
        return 0;
    }
    return 0;
}

/**
 * @brief adjust_wh_timer
 * @param wt
 * @param at
 * @return
 * reset timer, fristly, will remove timer, then insert wheel again.
 */
int adjust_wh_timer(WH_TIMERS *wt, AW_TIMER *at)
{
    if(wt && at)
    {
        del_wh_timer(wt, &at, 0);
        add_wh_timer(wt, at);
    }
}

/**
 * @brief wh_tick
 * @param wt
 * point move step by step, if slot is not null， run timeout callback function.
 * if loop cercle sum > 0, will skip it.
 */
void wh_tick(WH_TIMERS *wt)
{
    if(wt)
    {
        wt->curslotpoint = (wt->curslotpoint + 1)%N;
        if(wt->slot[wt->curslotpoint])
        {
            AW_TIMER * at = wt->slot[wt->curslotpoint];
            AW_TIMER *attemp = at;
            while(at)
            {
                attemp = at->next;
                if(0 >= at->loopcercle){
                    at->timeout_callback(at->data);
                    if(at->persist)
                    {
                        adjust_wh_timer(wt, at);
                    }else{
                        del_wh_timer(wt, &at, 1);
                    }
                }else{
                    (at->loopcercle)--;
                    break;
                }
                at = attemp;
            }
        }
        alarm(wt->timegap);
    }
}

/**
 * @brief destory_wh_timer
 * @param wt
 * free timer wheel.
 */
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


