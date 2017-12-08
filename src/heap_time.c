#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap_time.h"
static void swap(Timer *t1, Timer *t2)
{
	Timer temp;
	if (t1 && t2)
	{
		temp.timeout = t1->timeout;
		temp.data = t1->data;
		t1->timeout = t2->timeout;
		t1->data = t2->data;
		t2->timeout = temp.timeout;
		t2->data = temp.data;
	}
}
static void flow_up(HeapTimer *ht)
{
	if (ht)
	{
		if (ht->size <= 1)
		{
			return;
		}
		int temp = ht->size;
		while(temp)
		{
			if (ht->timer[temp/2].timeout > ht->timer[temp].timeout)
			{
				swap(&(ht->timer[temp/2]) , &(ht->timer[temp]));
			}else{
				break;
			}
			temp = temp/2;
		}
	}
}

static void flow_down(HeapTimer *ht)
{
	if (ht)
	{
		if (ht->size <= 1)
		{
			return;
		}
		int temp = 1;
		while(temp)
		{
			if ( ht->timer[2*temp].timeout < ht->timer[2*temp + 1].timeout)
			{
				swap(&(ht->timer[temp]), &(ht->timer[2*temp]));
				temp = 2*temp;
			}
			else{
				swap(&(ht->timer[temp]), &(ht->timer[2*temp + 1]));
				temp = 2*temp + 1;
			}
			if (temp >= ( (ht->size - 1)/2))
			{
				break;
			}
		}
	}
}

HeapTimer * initHeapTimer(size_t s)
{
	if (s <= (size_t)0)
	{
		return NULL;
	}
	HeapTimer * ht = (HeapTimer *)malloc(sizeof(HeapTimer));
	if (!ht)
	{
		return NULL;
	}
	ht->size = 0;
	ht->capacity  = s+1;
	ht->timer = (Timer *)malloc(sizeof(Timer) * (s+1));
	if (ht->timer)
	{
		return ht;
	}
	return NULL;
}

void min_heap_push(HeapTimer * ht, Timer * _timer)
{
	if (ht && _timer)
	{
		if (ht->size == ht->capacity)
		{
			size_t need = ht->size + (ht->size)/3;
			Timer *temp = (Timer *)malloc(sizeof(Timer)*(need));
			if(temp){
				memset(temp, 0, sizeof(Timer)*(need));
				memcpy(temp, ht->timer, sizeof(ht->size));
				ht->capacity = need;
				free(ht->timer);
				ht->timer = temp;
				temp = NULL;
			}
		}
		(ht->size)++;
		memcpy(&(ht->timer[ht->size]), _timer, sizeof(Timer));
		_timer = NULL;
		flow_up(ht);
	}
}

void min_heap_pop(HeapTimer * ht, Timer * _timer)
{
	if (_timer && ht && ht->size)
	{
		memset(_timer, 0, sizeof(Timer));
		memcpy(_timer, &(ht->timer[1]), sizeof(Timer));
		ht->timer[1].timeout = ht->timer[ht->size].timeout;
		ht->timer[1].data = ht->timer[ht->size].data;
		ht->timer[ht->size].timeout = 0;
		ht->timer[ht->size].data = NULL;
		(ht->size)--;
		flow_down(ht);
	}
}

int check_have_timeout(HeapTimer *ht, time_t now)
{
	if (ht)
	{
		if (ht->size > 0)
		{
			return (ht->timer[1]).timeout > now;
		}
	}
	return -1;
}


int main()
{
	HeapTimer * temp = initHeapTimer(100);
	Timer a = {11, NULL};
	Timer b = {34, NULL};
	Timer c = {22, NULL};
	Timer d = {3, NULL};
	Timer e = {33, NULL};
	Timer f = {333, NULL};
	Timer g = {10, NULL};
	min_heap_push(temp, &a);
	min_heap_push(temp, &b);
	min_heap_push(temp, &c);
	min_heap_push(temp, &d);
	min_heap_push(temp, &e);
	min_heap_push(temp, &f);
	min_heap_push(temp, &g);
	int i;
	for ( i = 0; i < 10; ++i)
	{
		Timer bb;
		min_heap_pop(temp, &bb);
		printf("%ld  ", bb.timeout);
	}
	printf("\n");
	return 0;
	
}