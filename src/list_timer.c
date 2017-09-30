#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "list_timer.h"

/**
 * @brief init_List_Timer
 * @param lt timer list
 * initialize list timer, head and tail are NULL
 */
void init_List_Timer(LIST_TIMER *lt)
{
    lt->timer_op.add = add_timer;
    lt->timer_op.del = del_timer;
    lt->timer_op.adjust = adjust_timer;
    lt->timer_op._tick = tick;
    lt->head = NULL;
    lt->tail = NULL;
}

/**
 * @brief add_timer
 * @param lt
 * @param ut
 * @return
 * Add a Timer in list, and timer in sort
 */
int add_timer(LIST_TIMER * lt, UTIL_TIME * ut)
{
    if (lt && ut)
    {
        if (!lt->head && !lt->tail)
        {
            lt->head = ut;
            lt->tail = ut;
            ut->prev = NULL;
            ut->next = NULL;
            return 1;
        }
        else if (lt->head && lt->tail)
        {
            UTIL_TIME * temp = lt->head;
            UTIL_TIME * tempbak = NULL;
            while(temp)
            {
                if((temp->out_time) > (ut->out_time))
                {
                    if(lt->head == temp)
                    {
                        ut->next = temp;
                        temp->prev = ut;
                        lt->head = ut;
                        ut->prev = NULL;
                        return 1;
                    }
                }
                tempbak = temp;
                temp = temp->next;
            }
            tempbak->next = ut;
            ut->prev = tempbak;
            ut->next = NULL;
            lt->tail = ut;
            return 1;
        }
        return 0;
    }
    return 0;
}

/**
 * @brief del_timer
 * @param lt
 * @param ut
 * @return
 * delete a timer, and free timer
 */
int del_timer(LIST_TIMER * lt, UTIL_TIME **ut)
{
    if(lt && ut && *ut)
    {
        if(lt->head)
        {
            UTIL_TIME *temp = lt->head;
            while(temp)
            {
                if(temp == *ut)
                {
                    if((temp == lt->head) && (temp != lt->tail))
                    {
                        lt->head = temp->next;
                        temp->next->prev = NULL;
                    }
                    else if((temp == lt->tail) && (temp != lt->head))
                    {
                        temp->prev->next = NULL;
                        lt->tail = temp->prev;
                    }
                    else if((temp == lt->tail) && (temp == lt->head))
                    {
                        lt->head = NULL;
                        lt->tail = NULL;
                    }
                    else
                    {
                        temp->next->prev = temp->prev;
                        temp->prev->next = temp->next;
                    }
                    (*ut)->cdata = NULL;
                    (*ut)->next = NULL;
                    (*ut)->prev = NULL;
                    (*ut)->timeout_callback = NULL;
                    free(*ut);
                    *ut = NULL;
                    ut = NULL;
                    return 1;
                }
                temp = temp->next;
            }
        }
    }
    return 0;
}

/**
 * @brief adjust_timer
 * @param lt
 * @param ut
 * @param _time
 * @return
 * if a timer be deleted or addition time to a timer, adjust timer list
 */
int adjust_timer(LIST_TIMER * lt, UTIL_TIME * ut, time_t _time)
{
    if(!lt || !ut){
        return 0;
    }
    ut->out_time = time(NULL) + _time;
    if(!ut->prev && !ut->next)
    {
        return 1;
    }else if (ut->prev && !ut->next) {
        ut->prev->next = NULL;
        ut->prev = NULL;
    }else if (!ut->prev && ut->next) {
        lt->head = ut->next;
        ut->next->prev = NULL;
        ut->next = NULL;
        ut->prev = NULL;
    }else{
        ut->next->prev = ut->prev;
        ut->prev->next = ut->next;
        ut->next = NULL;
        ut->prev = NULL;
    }
    if(add_timer(lt, ut))
    {
        return 1;
    }
    return 0;
}

/**
 * @brief tick
 * @param lt
 * Timer list tick, depend on callback function, adjust timer.
 */
void tick(LIST_TIMER * lt)
{
    if(lt){
        time_t now = time(NULL);
        UTIL_TIME *temp = lt->head;
        UTIL_TIME *tempbak = temp;
        while(temp)
        {
            unlock = 0;
            tempbak = temp;
            temp = temp->next;
            if(tempbak->out_time <= now)
            {
                small = 0;
                int res = tempbak->timeout_callback(tempbak->cdata);
                if(!res)
                    del_timer(lt, &tempbak);
                else{
                    adjust_timer(lt, tempbak, 10);
                }
            }
        }
        return;
    }
    return;
}

/**
 * @brief destroy_list_Timer
 * @param lt
 * destroy timer list
 */
void destroy_list_Timer(LIST_TIMER *lt)
{
    if(!lt){
        return;
    }
    UTIL_TIME * temp = lt->head;
    UTIL_TIME * tempnext = NULL;
    while(temp){
        tempnext = temp->next;
        temp->cdata = NULL;
        temp->next = NULL;
        temp->prev = NULL;
        temp->timeout_callback = NULL;
        free(temp);
        temp = tempnext;
    }
    lt->head = NULL;
    lt->tail = NULL;
}

LIST_TIMER lt;

void dotick(int arg)
{
    fprintf(stderr, ">>>>>><<<<<<\n");
    arg = 0;
    tick(&lt);
    alarm(1);
}

int doit(CLIENTDATA * data)
{
    if(data){
        switch (data->flags) {
        case CONNECTED:
            fprintf(stderr, "don`t delete\n");
            return 1;
        case READ:
            fprintf(stderr, "delete \n");
            return 0;
        case WROTE:
            break;
        case NORMAL:
            break;
        default:
            break;
        }
    }
    return 0;
}

int main()
{
    signal(SIGALRM, dotick);
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

    UTIL_TIME *ut1 = (UTIL_TIME *)malloc(sizeof(UTIL_TIME));
    ut1->timeout_callback = doit;
    ut1->cdata = cl1;
    ut1->out_time = time(NULL)+ 6;

    add_timer(&lt, ut1);
    add_timer(&lt, ut2);
    alarm(1);
    while(1){
        sleep(80);
    }
    return 0;
}
